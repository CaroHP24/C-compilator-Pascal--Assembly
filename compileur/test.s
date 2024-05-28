			# This code was produced by the CERI Compiler
.data
FormatString1:
	.string "%llu"
a:	.quad 0
b:	.quad 0
c:	.double 0.0
d:	.byte 0
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $2
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
	pop %rsi	# The value to be displayed
	movq $FormatString1, %rdi	# "%llu\n"
	movl	$0, %eax
	push %rbp 	#save the value in %rbp
	call	printf@PLT
	 pop %rbp
	movq $0, %rax
	movb $'d',%al
	push %rax	# push a 64-bit version of 'd'
	pop %rax
	movb %al,d
	push d
	pop %rsi	# The value to be displayed
	movq $FormatString1, %rdi	# "%llu\n"
	movl	$0, %eax
	push %rbp 	#save the value in %rbp
	call	printf@PLT
	 pop %rbp
