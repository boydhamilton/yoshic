global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 100
	push rax
	push QWORD [rbp -8]
	mov rax, 5
	push rax
	pop rbx
	pop rax
	xor rdx, rdx
	div rbx
	push rax
	push QWORD [rbp -16]
	mov rax, 9
	push rax
	mov rax, 8
	push rax
	pop rax
	pop rbx
	mul rbx
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	mov rax, 6
	push rax
	mov rax, 3
	push rax
	pop rbx
	pop rax
	xor rdx, rdx
	div rbx
	push rax
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	push QWORD [rbp -24]
	push QWORD [rbp -16]
	mov rax, 10
	push rax
	pop rbx
	pop rax
	xor rdx, rdx
	div rbx
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
