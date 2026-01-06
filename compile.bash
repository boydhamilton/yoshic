

./brhcc $1 $1.s
nasm -felf64 $1.s
ld -o $1.exe $1.o