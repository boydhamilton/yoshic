global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 1
	push rax
	mov rax, 3
	push rax
	push QWORD [rbp -8]
	push QWORD [rbp -16]
	mov rax, 7
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	push QWORD [rbp -24]
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
