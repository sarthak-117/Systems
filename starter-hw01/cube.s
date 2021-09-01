.globl cube
.text 

cube:
	enter $0, $0
	mov %rdi, %rax
	mul %rdi
	mul %rdi
	leave 
	ret


