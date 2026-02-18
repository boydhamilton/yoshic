# arch notes

## variables
- using stack, keep track of each variables position on the stack so at runtime we can retrieve it with sp
- cant pop off stack unless its on top, but we can peek at it 
- then, copy the value and push the copy onto top of the stack so some statement could use it (pop it)
- we use our own push and pop methods for registers so we can track stack size
- also rn all variables are 10 bits due to default imm val for x86. i am 99% sure that is why.

## scope
- vector of variables, so can linear search & get location to do math to find position in stack & frame it up ig ?!?

## stdlib
- first we need to add functions lol
- also strings
- arrays (?) 

then:
1) link against libc 
2) yoshi std functions compile to asm calls to libc functions (write stdlib.ys then have a static stdlib.s -> stdlib.o)
3) ensure the .s has extern decl for functions, as they will be defined in the compilation of stdlib.ys

rn, this layer should handle
- os i/o (console & files)
- heap memory