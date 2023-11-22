/*
 * Modified Brainfuck compiler.
 * Jackson Frey 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int debug = 0;
    if (argc > 2) {
        if (strcmp(argv[2], "-debug") == 0) {
            debug = 1;
        }
    }

    FILE* fptr;

    enum outmode output = CHAR;

    int cells[MAX] = {0};
    int ptr = 0;
    
    loop* loops[MAX] = {NULL};
    int lc = 0;
    
    func* funcs[MAX];
    int fc = 0;

    int returns[MAX];
    int layer = 0;

    fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        printf("Error: failed to open file");
        return 1;
    }

    while (!feof(fptr)) {
        char c = fgetc(fptr);

        if (debug) printf("Current Position: %ld\n", ftell(fptr));

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
        else if (c == '!') {
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
            //fscanf(fptr, "%s", str);
            fread(str, size, 1, fptr);

            cells[ptr] = size;
            ptr++;
            for (int i = 0; i < size; i++) {
                // Look for backslash
                if (str[i] == '\\') {
                    // Found '\n'
                    if (str[i+1] == 'n') {
                        cells[ptr] = 10;
                        ptr++;
                        i++;
                    }
                    else {
                        cells[ptr] = str[i];
                        ptr++;
                    }
                }
                else {
                    cells[ptr] = str[i];
                    ptr++;
                }
            }
        }

        // Declares a new function.
        else if (c == 'D') {
            fseek(fptr, 1, SEEK_CUR); // Skip space
           
            // Get the Function Name -------
            int start = ftell(fptr);
            char current;
            while (current != ' ') {
                current = fgetc(fptr);
            }
            int end = ftell(fptr);
            int size = end - start;
            char* func_name = malloc(size);
            fseek(fptr, start, SEEK_SET);
            fgets(func_name, size, fptr);
            // -----------------------------
      
            // Create reference to function and
            // add to function array.
            fseek(fptr, end+1, SEEK_SET);
            func* newFunc = malloc(sizeof(func));
            newFunc->name = func_name;
            newFunc->start = ftell(fptr);
            funcs[fc] = newFunc;
            fc++;

            if (debug) printf("Function Start: %d\n", newFunc->start);

            // Skip to the end of the defined function
            current = fgetc(fptr);
            while (current != ';') {
                current = fgetc(fptr);
            }
        }

        // Calls a function.
        else if (c == 'F') {
            fseek(fptr, 1, SEEK_CUR); // Skip parenthesis
            
            // Get Function Name ----------
            int start = ftell(fptr);
            char current = fgetc(fptr);
            while (current != ')') {
                current = fgetc(fptr);
            }
            int end = ftell(fptr);
            int size = end - start;
            char* func_name = malloc(size);
            fseek(fptr, start, SEEK_SET);
            fgets(func_name, size, fptr);
            // ----------------------------

            for (int i = 0; i < fc; i++) {
                func* f = funcs[i];

                if (!strcmp(f->name, func_name)) {
                    // Functions are ran JIT.
                    // We literally just move the file pointer
                    // back to the position in the file where the 
                    // function was defined, and run that code.
                    
                    layer++;
                    returns[layer] = end;
                    fseek(fptr, f->start, SEEK_SET);
                    fseek(fptr, -1, SEEK_CUR);
                    break;
                }
            }

            free(func_name);
        }

        // Returns program to position after function all.
        else if (c == ';') {
            fseek(fptr, returns[layer], SEEK_SET);
            returns[layer] = 0;
            layer--;
            if (debug) printf("Return Position: %ld\n", ftell(fptr));
        }

        // Copies current cell value to specified cell
        else if (c == '*') {
            int pos;
            fscanf(fptr, "%d", &pos);
            cells[pos] = cells[ptr];
        }

        // Places the current cell number/address into the specified cell
        else if (c == '@') {
            int pos;
            fscanf(fptr, "%d", &pos);
            cells[pos] = ptr;
        }
    }

    fclose(fptr);
    return 0;
}
