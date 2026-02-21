

# yoshi 

toy compiler for the yoshi programming language, something i came up with (largely) in libraries during reading week. read `notes/grammar.md` for information on how to write it!

targets x86 architecture, recommended to run in wsl enviroment.


## getting started
1) run makefile to generate `yoshic` executable 
2) write `program.ys`
   
    e.x. 
    ```
    let a = 10;
    let b = 9;
    if(a != b){
        exit 0;
    }
    exit a - b;
    ```
3) run `bash compile.bash program.ys`
4) run the generated `program.ys.exe`!

## bigger example
example binary search for some $x \in \{left...right\} \text{ s.t. } x \in \mathbf{N}$  where the program exits with the number of iterations it had to try. 

```
let right = 100;
let left = 0;
let x = 75;

let iters = 0;

let mid = 0;

while (left < right) {
    iters = iters + 1;

    mid = left + (right - left) / 2;

    if ( mid == x ) {
        exit iters;
    }

    if (mid < x) {
        left = mid + 1;
    }
	if (mid > x){
        right = mid - 1;
    }
}

if( left == right){
    iters = iters + 1;
	mid = left + (right - left) / 2;

    if ( mid == x ) {
        exit iters;
    }

    if (mid < x) {
        left = mid + 1;
    }
	if (mid > x){
        right = mid - 1;
    }
    
}

exit -1;
```

p.s. run `echo $?` to print exit value of last executed program in linux
