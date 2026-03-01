

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


# how to optimize compiler runtime

c++ -> fast c++

> note: this is not (necessarily) about generating a faster executable. it's about making the compiler generate a `.s` from a `.ys` or whatever as quickly as possible 

all benchmarking done with [Google Benchmark](https://github.com/google/benchmark).

```
Run on (12 X 2611.2 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 1280 KiB (x6)
  L3 Unified 12288 KiB (x1)
```

## first steps

first things first, we have to get a baseline to measure our improvements against. let's setup benchmark and see our results!

```
BM_Tokenizer         10024 ns         9768 ns        70388
bench.bash: line 8: 23858 Segmentation fault      (core dumped) ./benchmark_game
```

uh oh, looks like something went wrong! this is likely due to `-O3` optimization flag, which often takes advantage of undefined behaviour. however, this can get sticky if we were also, (albeit inadvertently!), taking advantage of it. so let's fix it.

