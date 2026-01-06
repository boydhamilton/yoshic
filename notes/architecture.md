# arch notes

## variables
- using stack, keep track of each variables position on the stack so at runtime we can retrieve it with sp
- cant pop off stack unless its on top, but we can peek at it 
- then, copy the value and push the copy onto top of the stack so some statement could use it (pop it)
- we use our own push and pop methods for registers so we can track stack size