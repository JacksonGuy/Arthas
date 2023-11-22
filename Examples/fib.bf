// Fibonacci program in Arthas
// Jackson Frey

!           // Change output mode

%3 = 25     // How many numbers to calculate (from 0)
            // For example, if you want 10 numbers, put 9

%4 = 32     // Space Character (for printing)

%5 = 0      // Starting number for fibonacci
%6 = 1

// Sums cells 5 and 6, result stored in cell 8
D add %5*8 %6*9 [%8+>-];

// Fibbonacci loop
D fib [ F(add) %6*5 %8.*6 %4!.!<-];

// Print first 2 numbers (our starting numbers)
%5. %4!.! 
%6. %4!.!

F(fib) 

%0=10!.     // Newline on finish
