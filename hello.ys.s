global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 1
	push rax
	mov QWORD [rbp - 8], rax
	push QWORD [rbp - 8]
	pop rax
	cmp rax, 0
	je else_1
	mov rax, 8
	push rax
	mov rax, 60
	pop rdi
	syscall
	jmp end_if_1
else_1:
end_if_1:
	mov rax, 1
	push rax
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
