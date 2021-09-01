.text
.global main

# mappings:
# %rdi = argc -> %r10
# %rsi = argv -> %r12, %r13 

#call atol and move value back to %r13
#ret 0, %rax == 0

#have one variable to worry about 

main:
#enter with 2 args, one is argc = rdi, other is argv = rsi
	enter $0, $0
	mov %rsi, %r13
	cmp $2, %rdi
	jne illegal_output

	mov 8(%r13), %rdi
	call atol
	mov %rax, 8(%r13)	

	cmp $0, 8(%r13)
	jl illegal_output
	
	mov 8(%r13), %rdi
	call fib
	
	mov $output_msg, %rdi 
	mov 8(%r13), %rsi
	mov %rax, %rdx
	mov $0, %al
	call printf
	leave
	ret

illegal_output: 
	mov $usage_msg, %rdi
	mov 0(%r13), %rsi
	mov $0, %al
	call printf
	leave
	ret

# enter with 16, move the stack to be able to store the rdi and fib output 
# check to see if rdi input is 0 or 1 for base cases
# calculate by calling fib on rdi - 1 and then storing it in the stack



#long rdi -> take in the value we are trying to call fib on 
#long rax -> the fib output 
fib:
	enter $16, $0
	cmp $0, %rdi
	je fib_done_0
	
	cmp $1, %rdi
	je fib_done_1

	mov %rdi, 0(%rsp)
	dec %rdi
	call fib
	mov %rax, 8(%rsp)
	
	mov 0(%rsp), %rdi
	dec %rdi
	dec %rdi
	call fib
	add 8(%rsp), %rax

	leave 
	ret

fib_done_0:
	mov $0, %rax
	leave
	ret
fib_done_1:
	mov $1, %rax
	leave
	ret
	


.text
usage_msg: .asciz "Usage:\n  %s N, where N > 0\n"
output_msg: .asciz "fib(%ld) = %ld\n" 

