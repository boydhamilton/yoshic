global _start
_start:
	push rbp
	mov rbp, rsp

	jmp funct_dime_end
funct_dime:
	mov rsp, rbp
	pop rbp
	pop rax
	ret
	add rsp, 0
	mov rsp, rbp
	pop rbp
	ret
funct_dime_end:
	call funct_dime
	push rax
	mov rax, 0
	push rax
	mov rax, 60
	pop rdi
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
