//.global swap
//.global partition
//.global quicksort
//.global read_int
.global main
.text


//xs -> %r12
//nn -> 0(%rsi)
//pass along nn mem location to the read_int
//ii -> %r14 (calling a function so %rcx would be wiped)
main:
	push %r12
	push %r13
	push %r14
	push %r15
	enter $16, $0

	lea (%rsp), %rdi
	mov $0, %al
	call read_int

	mov 0(%rsp), %r13
	mov %r13, %rax
	mov $8, %r10
	imul %r10
	mov %rax, %rdi
	call malloc
	mov %rax, %r12 #holds xs
	mov $0, %r14
	jmp main_loop_read

main_loop_read:
	cmp %r13, %r14
	jge read_loop_end
	lea (%r12,%r14,8), %rdi
	call read_int
	add $1, %r14
	jmp main_loop_read

read_loop_end:
	mov %r12, %rdi
	mov $0, %rsi
	mov %r13, %rdx
	call quicksort
	
	mov $input_string, %rdi
	mov %r13, %rsi
	call printf
	mov $0, %r14
	jmp main_loop_out

main_loop_out:
	cmp %r13, %r14
	jge out_loop_end
	mov $output_string, %rdi
	mov (%r12, %r14,8), %rsi
	call printf
	add $1, %r14 
	jmp main_loop_out

out_loop_end:
	mov $print, %rdi
	call printf
	mov %r12, %rdi
	call free
	mov $0, %rax
	leave
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	ret

// one argument taken in, void method
// %rdi == &nn, mov to %r10 (temp register to mov to %rsi)
// mov %ld to %rdi, call scanf
read_int:
	enter $0, $0 
	mov %rdi, %r10
	mov $int_string, %rdi
	mov %r10, %rsi
	mov $0, %al
	call scanf
	cmp $1, %rax
	jne bad_exit	
	leave 
	ret
		
// bad exit if there are more than two longs being read by scanf
bad_exit:
	mov $bad_output, %rdi
	call puts
	mov $1, %rdi
	call exit
	leave 
	ret //dont know if necessary


// enter with 3 args
// xs -> %rdi
// ii -> %rsi
// yy -> %rdx
// tmp -> %r10
// tmp 2 that allows for swap -> %r11
swap:  
	enter $0, $0
	cmp %rsi, %rdx
	je end_swap
	
	mov (%rdi, %rsi, 8), %r10
	mov (%rdi, %rdx, 8), %r11
	mov %r11,  (%rdi, %rsi, 8) #maybe mov into a temp register
	mov %r10, (%rdi, %rdx, 8)

	jmp end_swap
	
end_swap:
	leave 
	ret


//enter with 3 args, return with long in %rax
// xs -> %rdi -> %r12
// lo -> %rsi -> ii -> 0(%rsp)
// hi -> %rdx -> %r13
// pivot -> %r14
// yy -> %r15
// last step is mov 0(%rsi), %rax

partition:
	push %r12
	push %r13
	push %r14
	push %r15
	enter $8, $0
	mov %rdi, %r12 //xs
	mov %rdx, %r13 //hi
	sub $1,  %r13 //hi - 1
	mov (%rdi, %r13, 8), %r14 //pivot

	mov %rsi, %r15 #yy
	mov %rsi, 0(%rsp) #ii
	jmp partition_main

partition_main:
	cmp %r13, %r15
	jge partition_end

	mov (%r12, %r15, 8), %r10
	cmp %r14, %r10
	jge partition_help
	
	mov %r12, %rdi
	mov 0(%rsp), %rsi
	mov %r15, %rdx
	call swap
	add $1, 0(%rsp)
	jmp partition_help
	
partition_help:
	add $1, %r15
	jmp partition_main
	
partition_end:
	mov %r12, %rdi
	mov 0(%rsp), %rsi
	mov %r15, %rdx
	call swap
	mov 0(%rsp), %rax
	leave
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	ret


// enter with 3 args, exit with none
// xs-> %r12, lo -> %r13, hi -> %r14 pp -> %r15	
quicksort: 
	push %r12
	push %r13
	push %r14
	push %r15
	enter $0, $0
	mov %rdi, %r12
	mov %rsi, %r13
	mov %rdx, %r14
	
	mov %r14, %rax
	sub %r13, %rax
	
	cmp $1, %rax
	jl quicksort_end
	
	mov %r12, %rdi
	mov %r13, %rsi
	mov %r14, %rdx
	call partition

	mov %rax, %r15 //long pp lmao
	mov %r12, %rdi
	mov %r13, %rsi
	mov %r15, %rdx
	call quicksort
	
	add $1, %r15
	mov %r12, %rdi
	mov %r15, %rsi
	mov %r14, %rdx
	call quicksort

quicksort_end: 
	leave
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	ret

	


.text
int_string: .string "%ld"
input_string: .asciz "%ld\n"
output_string: .asciz "%ld "
print: .asciz "\n"
bad_output: .string "bad output"

/*Signature
main: inputs -> argc -> %rdi, argv -> %rsi
returns an int in %rax, outputs to the screen
vars: nn -> %r12

read_int:  ptr = %rdi -> the memory location to the ptr
vars: ptr -> r10
learn how to syscall for exit with a bad input if the ptr is not valid

quicksort: long xs -> %rdi, long lo -> %rsi, long hi -> %rdx 
 vars: xs -> r12, lo -> 0(%rsp), hi -> 8(%rsp)

partition: inputs -> long ptr  
*/



