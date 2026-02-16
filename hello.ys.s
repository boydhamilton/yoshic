global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 0
	push rax
	mov QWORD [rbp - 8], rax
	push QWORD [rbp - 8]
	pop rax
	cmp rax, 0
	je else_0
	push QWORD [rbp - 8]
	mov rax, 60
	pop rdi
	syscall
	jmp end_if_0
else_0:
end_if_0:
	mov rax, 100
	push rax
	mov QWORD [rbp - 16], rax
	push QWORD [rbp - 16]
	pop rax
	cmp rax, 0
	je else_1
	push QWORD [rbp - 8]
	push QWORD [rbp - 16]
	pop rax
	pop rbx
	add rax, rbx
	push rax
	pop rax
	mov QWORD [rbp - 8], rax
	jmp end_if_1
else_1:
end_if_1:
	add rsp, 8
	push QWORD [rbp - 8]
	pop rax
	cmp rax, 0
	je else_2
	push QWORD [rbp - 8]
	mov rax, 60
	pop rdi
	syscall
	jmp end_if_2
else_2:
end_if_2:

	mov rax, 60
	mov rdi, 0
	syscall
