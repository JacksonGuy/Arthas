// Fibonacci program in Arthas

! // Change output mode

%3 = 10 // How many numbers to calculate
%4 = 10 // Newline Character (for later)

// Starting number for fibonacci
%5 = 0
%6 = 1

// Sums cells 5 and 6, result stored in cell 8
D add %5*8 %6*9 [%8+>-];

// Fibbonacci loop
D fib [ F(add) %8.*5 %4!.!<- F(add) %8.*6 %4!.!<- ];

%5. %4!.!
%6. %4!.!

F(fib)
