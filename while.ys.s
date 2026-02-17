global _start
_start:
	push rbp
	mov rbp, rsp

	mov rax, 100
	push rax
	mov QWORD [rbp - 8], rax
	mov rax, 0
	push rax
	mov QWORD [rbp - 16], rax
	mov rax, 75
	push rax
	mov QWORD [rbp - 24], rax
	mov rax, 0
	push rax
	mov QWORD [rbp - 32], rax
	mov rax, 0
	push rax
	mov QWORD [rbp - 40], rax
while_loop_start_l0:
	push QWORD [rbp - 16]
	push QWORD [rbp - 8]
	pop rbx
	pop rax
	cmp rax, rbx
	setl al
	movzx rax, al
	push rax
	pop rax
	cmp rax, 0
	je while_loop_end_l0
	push QWORD [rbp - 32]
	mov rax, 1
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	pop rax
	mov QWORD [rbp - 32], rax
	push QWORD [rbp - 16]
	push QWORD [rbp - 8]
	push QWORD [rbp - 16]
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	mov rax, 2
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
	pop rax
	mov QWORD [rbp - 40], rax
	push QWORD [rbp - 40]
	push QWORD [rbp - 24]
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	pop rax
	cmp rax, 0
	sete al
	movzx rax, al
	push rax
	pop rax
	cmp rax, 0
	je else_l1
	push QWORD [rbp - 32]
	mov rax, 60
	pop rdi
	syscall
	jmp end_if_l1
else_l1:
end_if_l1:
	push QWORD [rbp - 40]
	push QWORD [rbp - 24]
	pop rbx
	pop rax
	cmp rax, rbx
	setl al
	movzx rax, al
	push rax
	pop rax
	cmp rax, 0
	je else_l2
	push QWORD [rbp - 40]
	mov rax, 1
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	pop rax
	mov QWORD [rbp - 16], rax
	jmp end_if_l2
else_l2:
end_if_l2:
	push QWORD [rbp - 40]
	push QWORD [rbp - 24]
	pop rbx
	pop rax
	cmp rax, rbx
	setg al
	movzx rax, al
	push rax
	pop rax
	cmp rax, 0
	je else_l3
	push QWORD [rbp - 40]
	mov rax, 1
	push rax
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	pop rax
	mov QWORD [rbp - 8], rax
	jmp end_if_l3
else_l3:
end_if_l3:
	jmp while_loop_start_l0
while_loop_end_l0:
	push QWORD [rbp - 16]
	push QWORD [rbp - 8]
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	pop rax
	cmp rax, 0
	sete al
	movzx rax, al
	push rax
	pop rax
	cmp rax, 0
	je else_l4
	push QWORD [rbp - 32]
	mov rax, 1
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	pop rax
	mov QWORD [rbp - 32], rax
	push QWORD [rbp - 16]
	push QWORD [rbp - 8]
	push QWORD [rbp - 16]
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	mov rax, 2
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
	pop rax
	mov QWORD [rbp - 40], rax
	push QWORD [rbp - 40]
	push QWORD [rbp - 24]
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	pop rax
	cmp rax, 0
	sete al
	movzx rax, al
	push rax
	pop rax
	cmp rax, 0
	je else_l5
	push QWORD [rbp - 32]
	mov rax, 60
	pop rdi
	syscall
	jmp end_if_l5
else_l5:
end_if_l5:
	push QWORD [rbp - 40]
	push QWORD [rbp - 24]
	pop rbx
	pop rax
	cmp rax, rbx
	setl al
	movzx rax, al
	push rax
	pop rax
	cmp rax, 0
	je else_l6
	push QWORD [rbp - 40]
	mov rax, 1
	push rax
	pop rax
	pop rbx
	add rax, rbx
	push rax
	pop rax
	mov QWORD [rbp - 16], rax
	jmp end_if_l6
else_l6:
end_if_l6:
	push QWORD [rbp - 40]
	push QWORD [rbp - 24]
	pop rbx
	pop rax
	cmp rax, rbx
	setg al
	movzx rax, al
	push rax
	pop rax
	cmp rax, 0
	je else_l7
	push QWORD [rbp - 40]
	mov rax, 1
	push rax
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	pop rax
	mov QWORD [rbp - 8], rax
	jmp end_if_l7
else_l7:
end_if_l7:
	jmp end_if_l4
else_l4:
end_if_l4:
	mov rax, 1
	push rax
	pop rax
	neg rax
	push rax
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
