// Hello world, using Arthas Features

// Print string at current address
D print *0>@1<@2%0[>&.%1+<-]%2&;

%2 = 10              // Newline Character

%10S"Hello World!\n" // Declare String
%10                  // Goto String Start
F(print)             // Call Print Function
