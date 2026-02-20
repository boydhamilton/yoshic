

# yoshi 

compiler for the yoshi programming language, created and developed by me 

Example binary search: 

```

let right = 100;
let left = 0;
let x = 75;

let iters = 0;

let mid = 0;

while (left < right) {
    iters = iters + 1;

    mid = left + (right - left) / 2;

    if ( !(mid - x) ) {
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

    if ( !(mid - x) ) {
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