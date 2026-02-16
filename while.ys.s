global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 10
	push rax
	mov QWORD [rbp - 8], rax
	mov rax, 1
	push rax
	mov QWORD [rbp - 16], rax
while_loop_start_l0:
	push QWORD [rbp - 8]
	pop rax
	cmp rax, 0
	je while_loop_end_l0
	push QWORD [rbp - 16]
	mov rax, 2
	push rax
	pop rax
	pop rbx
	mul rbx
	push rax
	pop rax
	mov QWORD [rbp - 16], rax
	push QWORD [rbp - 8]
	mov rax, 1
	push rax
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	pop rax
	mov QWORD [rbp - 8], rax
	jmp while_loop_start_l0
while_loop_end_l0:
	mov rax, 3
	push rax
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
