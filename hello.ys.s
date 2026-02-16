global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 0
	push rax
	mov QWORD [rbp - 8], rax
	mov rax, 200
	push rax
	mov QWORD [rbp - 16], rax
	mov rax, 100
	push rax
	mov QWORD [rbp - 24], rax
	mov rax, 100
	push rax
	mov QWORD [rbp - 32], rax
	push QWORD [rbp - 16]
	push QWORD [rbp - 24]
	pop rax
	pop rbx
	add rax, rbx
	push rax
	push QWORD [rbp - 32]
	pop rax
	pop rbx
	add rax, rbx
	push rax
	mov QWORD [rbp - 40], rax
	push QWORD [rbp - 40]
	pop rax
	mov QWORD [rbp - 8], rax
	add rsp, 32
	mov rax, 200
	push rax
	mov QWORD [rbp - 16], rax
	mov rax, 300
	push rax
	mov QWORD [rbp - 24], rax
	mov rax, 999
	push rax
	mov QWORD [rbp - 32], rax
	push QWORD [rbp - 8]
	push QWORD [rbp - 16]
	pop rbx
	pop rax
	xor rdx, rdx
	div rbx
	push rax
	mov rax, 60
	pop rdi
	syscall
	add rsp, 8
	push QWORD [rbp - 16]
	push QWORD [rbp - 24]
	pop rax
	pop rbx
	add rax, rbx
	push rax
	pop rax
	mov QWORD [rbp - 8], rax
	add rsp, 16
	mov rax, 100
	push rax
	mov QWORD [rbp - 16], rax
	mov rax, 1
	push rax
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
