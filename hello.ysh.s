global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 3
	push rax
	mov rax, 3
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	mov rax, 3
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	mov rax, 1
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	mov rax, 2
	push rax
	pop rbx
	pop rax
	xor rdx, rdx
	div rbx
	push rax
	push QWORD [rbp -8]
	mov rax, 5
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	push QWORD [rbp -8]
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
