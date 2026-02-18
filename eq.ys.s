global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 10
	push rax
	mov QWORD [rbp - 8], rax
	jmp funct_rewritex_end
funct_rewritex:
	mov QWORD [rbp - 16], rdi
	push QWORD [rbp - 16]
	pop rax
	mov QWORD [rbp - 8], rax
	add rsp, 8
	jmp funct_rewritex_end
funct_rewritex_end:
	mov rax, 3
	push rax
	pop rdi
	call funct_rewritex
	push rax
	push QWORD [rbp - 8]
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
