			# This code was produced by the CERI Compiler
.data
FormatString1:
	.string "%llu\n"
FormatString2:
	.string "%lf\n"
FormatString3:
	.string "%c\n"
TrueString:	.string "TRUE"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE"	# used by printf to display the boolean value FALSE
a:	.quad 0
b:	.quad 0
c:	.double 0.0
d:	.byte 0
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $2
	push $2
	pop %rbx
	pop %rax
	mulq %rbx
	push %rax	# MUL
	pop a
	push a
	pop %rsi	# The value to be displayed
	movq $FormatString1, %rdi	# "%llu\n"
	movl	$0, %eax
	push %rbp 	#save the value in %rbp
	call	printf@PLT
	 pop %rbp
	push $5
	pop b
	push b
	pop %rsi	# The value to be displayed
	movq $FormatString1, %rdi	# "%llu\n"
	movl	$0, %eax
	push %rbp 	#save the value in %rbp
	call	printf@PLT
	 pop %rbp
	subq $8,%rsp			# allocate 8 bytes on stack's top
	movl	$0, (%rsp)	# Conversion of 2.5 (32 bit high part)
	movl	$1074003968, 4(%rsp)	# Conversion of 2.5 (32 bit low part)
	pop c
	push c
	movsd (%rsp), %xmm0	# The value to be displayed
	addq $8, %rsp	# Remove the double value from the stack
	movq $FormatString2, %rdi	# "%lf\n"
	movl $1, %eax	# Number of floating-point arguments
	push %rbp 	# save the value in %rbp
	call printf@PLT
	pop %rbp
	movq $0, %rax
	movb $'d',%al
	push %rax	# push a 64-bit version of 'd'
	pop %rax
	movb %al,d
	push d
	pop %rsi	# The value to be displayed
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	push %rbp 	# save the value in %rbp
	call printf@PLT
	pop %rbp
	movq $0, %rax
	movb $'=',%al
	push %rax	# push a 64-bit version of '='
	pop %rsi	# The value to be displayed
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	push %rbp 	# save the value in %rbp
	call printf@PLT
	pop %rbp
	push a
	push $3
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	ja Vrai7	# If above
	push $0		# False
	jmp Suite7
Vrai7:	push $0xFFFFFFFFFFFFFFFF		# True
Suite7:
	pop %rdx	# Zero : False, non-zero : true
	cmpq $0, %rdx
	je False
	movq $TrueString, %rdi	# "TRUE\n"
	jmp Next
False:
	movq $FalseString, %rdi	# "FALSE\n"
Next:
	call	puts@PLT
	push $4
	pop %rax
	mov %rax, a
ForStart7:
	push $1
	pop %rbx
	mov a, %rax
	cmp %rbx, %rax
	jl ForEnd7
	push b
	push $1
	pop %rbx
	pop %rax
	addq %rbx, %rax	# ADD
	push %rax
	pop b
	push b
	pop %rsi	# The value to be displayed
	movq $FormatString1, %rdi	# "%llu\n"
	movl	$0, %eax
	push %rbp 	#save the value in %rbp
	call	printf@PLT
	 pop %rbp
EndBlock8:
	mov a, %rax
	sub $1, %rax
	mov %rax, a
	jmp ForStart7
ForEnd7:
	push b
	pop %rsi	# The value to be displayed
	movq $FormatString1, %rdi	# "%llu\n"
	movl	$0, %eax
	push %rbp 	#save the value in %rbp
	call	printf@PLT
	 pop %rbp
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
