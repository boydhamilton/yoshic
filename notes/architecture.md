# arch notes

## variables
- using stack, keep track of each variables position on the stack so at runtime we can retrieve it with sp
- cant pop off stack unless its on top, but we can peek at it 
- then, copy the value and push the copy onto top of the stack so some statement could use it (pop it)
- we use our own push and pop methods for registers so we can track stack size
- also rn all variables are 10 bits due to default imm val for x86. i am 99% sure that is why. some of them are 8, but not gonna deal with high/low registers or types rn

## scope
- vector of variables, so can linear search & get location to do math to find position in stack & frame it up ig ?!?