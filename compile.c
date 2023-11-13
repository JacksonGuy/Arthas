/*
 * Modified Brainfuck compiler.
 * Jackson Frey 2023
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX 128

typedef struct {
    int start;
    int end;
} loop;

typedef struct {
    char* name;
    int start;
} func;

enum outmode {
    INT,
    CHAR
};

int main(int argc, char* argv[]) {
    FILE* fptr;

    enum outmode output = CHAR;

    int cells[MAX] = {0};
    int ptr = 0;
    
    loop* loops[MAX] = {NULL};
    int lc = 0;
    
    func* funcs[MAX] = {NULL};
    int fc = 0;
    int returnPos = 0;

    fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        printf("Error: failed to open file");
        return 1;
    }

    while (!feof(fptr)) {
        char c = fgetc(fptr);

        if (c == '>') {
            if (ptr < MAX) {
                ptr++;
            }   
        }

        else if (c == '<') {
            if (ptr > 0) {
                ptr--;
            }
        }

        else if (c == '+') {
            cells[ptr]++;
        }

        else if (c == '-') {
            cells[ptr]--;
        }

        else if (c == '.') {
            if (output == CHAR) {
                printf("%c", cells[ptr]);
            }
            else {
                printf("%d", cells[ptr]);
            }
        }

        else if (c == ',') {
            scanf("%d", &cells[ptr]);
        }

        else if (c == '[') {
            if (loops[lc] == NULL) goto NEW;
            if (loops[lc]->start == ftell(fptr)) goto SKIP;

            NEW:
            loop* nl = malloc(sizeof(loop));
            nl->start = ftell(fptr);
            char end;
            while (end != ']') {
                end = fgetc(fptr);
            }
            nl->end = ftell(fptr);
            fseek(fptr, nl->start, SEEK_SET);
            lc++;
            loops[lc] = nl;

            SKIP:
            if (cells[ptr] == 0) {
                fseek(fptr, loops[lc]->end, SEEK_SET);
            }
        }

        else if (c == ']') {
            if (cells[ptr] != 0) {
                fseek(fptr, loops[lc]->start, SEEK_SET);   
            }
            else {
                free(loops[lc]);
                loops[lc] = NULL;
                lc--;
            }
        }
       
        // Comments
        else if (c == '/') {
            char next = fgetc(fptr);
            if (next == '/') {
                while (next != EOF) {
                    next = fgetc(fptr);
                    if (next == '\n') break;
                }
            }
        }

        // Changes compiler output mode.
        // This way, if we want to output integers, we
        // don't have to go through the trouble of setting
        // the cell value to the ASCII code of that number.
        else if (c == '@') {
            if (output == CHAR) {
                output = INT;
            }
            else {
                output = CHAR;
            }
        }

        // Sets cell pointer to specified location
        else if (c == '%') {
            int pos;
            fscanf(fptr, "%d", &pos);
            ptr = pos;
        }

        // Stores following value in current cell
        else if (c == '=') {
            char next = fgetc(fptr);
            if (next == '\'') {
                char ic = fgetc(fptr);
                cells[ptr] = ic;
            }
            else {
                fseek(fptr, -1, SEEK_CUR);
                int val;
                fscanf(fptr, "%d", &val);
                cells[ptr] = val;
            }
        }

        // Sets cell pointer to value of current cell
        else if (c == '&') {
            ptr = cells[ptr];
        }

        // Initialize String, starting at current cell.
        // Declare example: S"Hello, World!"
        // First cell: size
        // Second cell: start address
        // Rest: each char of string
        else if (c == 'S') {
            fseek(fptr, 1, SEEK_CUR); // Skip over first quote
            int start = ftell(fptr);
            char current;
            while (current != '"') {
                current = fgetc(fptr);
            }
            int end = ftell(fptr) - 1;
            int size = end-start;
            char str[size];
            fseek(fptr, start, SEEK_SET);
            fscanf(fptr, "%s", str);

            cells[ptr] = size;
            cells[ptr+1] = ptr+2; // TODO bad?
            ptr += 2;
            for (int i = 0; i < size; i++) {
                if (str[i] == '\\') {
                    if (str[i+1] == 'n') {
                        cells[ptr] = 10;
                        ptr += 2;
                    }
                }
                else {
                    cells[ptr] = str[i];
                    ptr++;
                }
            }
            ptr++;
        }

        // Declares a new function.
        else if (c == 'D') {
            fseek(fptr, 1, SEEK_CUR); // Skip space
           
            int start = ftell(fptr);
            char current;
            while (current != ' ') {
                current = fgetc(fptr);
            }
            int end = ftell(fptr) - 1;
            int size = end - start;
            char func_name[size];
            fseek(fptr, start, SEEK_SET);
            fscanf(fptr, "%s", func_name);
       
            // Create reference to function and
            // add to function array.
            //
            // TODO this is very bad.
            // We absolutely shouldn't be using an array for this,
            // but right now a HashSet seems overkill.
            fseek(fptr, end+1, SEEK_SET);
            func* newFunc = malloc(sizeof(func));
            newFunc->name = func_name;
            newFunc->start = ftell(fptr);
            funcs[fc] = newFunc;
            fc++;

            current = fgetc(fptr);
            while (current != ';') {
                current = fgetc(fptr);
            }
        }

        // Calls a function.
        else if (c == 'F') {
            fseek(fptr, 1, SEEK_CUR); // Skip parenthesis
            int start = ftell(fptr);
            char current;
            while (current != ')') {
                current = fgetc(fptr);
            }
            int end = ftell(fptr);
            int size = end - start;
            char func_name[size];
            fseek(fptr, start, SEEK_SET);
            fgets(func_name, size, fptr);

            for (int i = 0; i < fc; i++) {
                func* f = funcs[i];
                if (f->name == func_name) {
                    fseek(fptr, funcs[i]->start, SEEK_SET);
                    returnPos = end+1;
                    break;
                }
            }
        }

        // Returns program to position after function all.
        else if (c == ';') {
            fseek(fptr, returnPos, SEEK_SET);
            returnPos = 0;
        }

        // Copies current cell value to specificied cell
        else if (c == '*') {
            int pos;
            fscanf(fptr, "%d", &pos);
            cells[pos] = cells[ptr];
        }
    }

    fclose(fptr);
    return 0;
}
