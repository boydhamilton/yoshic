global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 21
	push rax
	mov rax, 39
	push rax
	push QWORD [rbp -8]

	push QWORD [rbp -16]

	push QWORD [rbp -24]

	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
