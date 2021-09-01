.global main
.text

main:
#have multiple args to worry about
#argc has to be 4
#argv[0] = function name
#argv[1] = first number
#argv[2] = operand
#argv[3] = second number
#store function name in r12
#store first number in r13
#store operand in r14
#store second number in r15

#cmp each case using strcmp, push operand to rsi and use the strings declared at the bottom to rdi to call this 

	push %r12
	push %r13
	push %r14
	push %r15
	enter $0, $0
	
	mov 0(%rsi), %r12
	mov 8(%rsi), %r13
	mov 16(%rsi), %r14
	mov 24(%rsi), %r15

	cmp $4, %rdi
	jne illegal_output
	
	mov %r13, %rdi
	call atol
	mov %rax, %r13

	mov %r15, %rdi
	call atol
	mov %rax, %r15	
	
	mov %r14, %rsi
	mov $add_operator, %rdi
	call strcmp
	cmp $0, %rax
	je add_two

	mov %r14, %rsi
	mov $subtract_operator, %rdi
	call strcmp
	cmp $0, %rax
	je subtract_two	

	mov %r14, %rsi
	mov $multiply_operator, %rdi
	call strcmp
	cmp $0, %rax
	je multiply_two

	mov %r14, %rsi
	mov $divide_operator, %rdi
	call strcmp
	cmp $0, %rax
	je divide_two

	jmp illegal_output
	

illegal_output: 
	mov $usage_msg, %rdi
	mov %r12, %rsi
	mov $0, %al
	call printf
	leave 
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	ret
#need values stored here (not really functions, more like conditional branches?
add_two:
	mov %r13, %rax
	add %r15, %rax
	
	mov $output_msg, %rdi
	mov %r13, %rsi
	mov %r14, %rdx
	mov %r15, %rcx
	mov %rax, %r8
	mov $0, %al
	call printf
	
	leave
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	ret

subtract_two:
	mov %r13, %rax
	sub %r15, %rax

	mov $output_msg, %rdi
	mov %r13, %rsi
	mov %r14, %rdx
	mov %r15, %rcx
	mov %rax, %r8
	mov $0, %al
	call printf

	leave 
	pop %r12
	pop %r13
	pop %r14
	pop %r15	
	ret

multiply_two:
	mov %r13, %rax
	imul %r15
	
	mov $output_msg, %rdi
	mov %r13, %rsi
	mov %r14, %rdx
	mov %r15, %rcx
	mov %rax, %r8
	mov $0, %al
	call printf


	leave
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	ret

divide_two:
	mov %r13, %rax
	idiv %r15
	mov $output_msg, %rdi
	
	mov %r13, %rsi
	mov %r14, %rdx
	mov %r15, %rcx
	mov %rax, %r8
	mov $0, %al
	call printf

	leave 
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	ret

.text
add_operator: .string "+"
subtract_operator: .string "-"
multiply_operator: .string "*"
divide_operator: .string "/"

usage_msg: .asciz "Usage:\n  %s N op M\n"
output_msg: .asciz "%ld %s %ld = %ld\n"
test_msg: .asciz "this works!"
	
