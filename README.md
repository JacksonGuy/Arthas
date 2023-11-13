# Arthas
A slightly more advanced Brainfuck-esque language.

## Why?

Because I got bored one night.<br>
Also, because Brainfuck is a cool language, except that it sucks to use. So, I decided to make it more practical.

## Features

### Comments
Unlike Brainfuck, and sadly the only feature which makes this compiler incapable of fully compiling Brainfuck files, is that
comments must follow a `//`.<br>
Example:<br>
`+>++>+++ // Puts 1,2,3 into memory`<br>

### Set Value
Probably the most useful feature, now you can insert numbers directly into memory using the `=` command.<br>
Example:<br>
`=65 // Sets cell 0 to 65`<br>

### Set Pointer Position
The Second most useful feature, rather than moving between cells one-by-one, you can now move to any cell immediately.<br>
Example:<br>
`%10=66 // Sets cell 10 to 66`<br> 

### Goto cell value
Moves the cell pointer to the value of the current cell.<br>
Example:<br>
```
%5=67 // Sets cell 5 to 67
>=5 // Sets cell 6 to 5
&. // Sets pointer to cell 5, then outputs its value (67)
```
<br>

### Character values
Good news! Now you don't need to remember every value of an ASCII table. We can now directly enter an ASCII character into a cell.<br>
Example:<br>
`='H'>='i'>='!' // Enters "Hi!" into three consecutive cells`

### Strings
Additionally, now we can just input entire strings into memory with ease.<br>
Example:<br>
`S"Hello, World!\n"`<br>
We begin a string with the `S` character, and enclose the string with quotes.<br>
Starting at the current cell, we first store the size of the string. Then, in the next cell, we store the the starting position of the string.<br>
In the above example, the program memory would look like this:<br>
`[15][2][72][101][108][108] (...)`<br>
The number 15 is the length of the string, and the number 2 is the cell of the first letter of the string.<br>

### Functions
Our most advanced feature, we can now declare functions!<br>
We declare functions as such:<br>
`D add_one +;`<br>
You start with the character D, then follow it with the name of the function, then the functions code, ending with a semicolon.<br>
I'll warn that the compiler is very picky about function declarations.
<ol>
    <li> The 'D' must be followed by a space. </li>
    <li> The function name cannot have a space in the name. Besides that, go wild </li>
    <li> The code portion <b>must</b> end with a semicolon. </li>
</ol><br>
To call functions:<br>
`F(add_one)`<br>
We use the `F` character, with the function name surrounded by parenthesis. After the function call, the program will return
to the position right after the last parenthesis (this is why you need the semicolon)(So I guess you don't <i>technically</i> need the semicolon).<br>

### Output modes
You now have the option of interpreting values in memory as regular integer numbers rather than ASCII characters.<br>
To switch between ASCII and INT output modes, use the `@` command.<br>
Example:<br>
```
=65 . // Outputs A
@ .   // Outputs 65
```
