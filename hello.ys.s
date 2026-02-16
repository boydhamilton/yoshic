global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 10
	push rax
	mov QWORD [rbp - 8], rax
	add rsp, 8
	mov rax, 100
	push rax
	mov QWORD [rbp - 8], rax
	push QWORD [rbp - 8]
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
