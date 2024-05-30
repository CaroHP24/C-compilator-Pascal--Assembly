			# This code was produced by the CERI Compiler
.data
FormatString1:
	.string "%llu\n"
FormatString2:
	.string "%lf\n"
FormatString3:
	.string "%c\n"
TrueString:	.string "TRUE"	#  TRUE
FalseString:	.string "FALSE"	#  FALSE
a:	.quad 0
b:	.quad 0
c:	.double 0.0
d:	.byte 0
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
DebutBlock0:
	push $4
	pop a
	push $5
	pop b
	subq $8,%rsp			# allocate 8 bytes on stack's top
	movl	$0, (%rsp)	# Conversion of 2.5 (32 bit high part)
	movl	$1074003968, 4(%rsp)	# Conversion of 2.5 (32 bit low part)
	pop c
	movq $0, %rax
	movb $'d',%al
	push %rax	# push a 64-bit version of 'd'
	pop %rax
	movb %al,d
DebutCase1:
	push a
	pop %rbx	# valeur à comparer dans case
Case2:
	push $2
	pop %rax
	cmp %rbx, %rax
	jne Case3
	push $2
	pop b
	jmp EndCase1
Case3:
	push $6
	pop %rax
	cmp %rbx, %rax
	jne Case4
	push $22
	pop b
	jmp EndCase1
Case4:
	push $28
	pop b
EndCase1:
	push b
	pop %rsi	# The value to be displayed
	movq $FormatString1, %rdi	# "%llu\n"
	movl	$0, %eax
	push %rbp 	#save the value in %rbp
	call	printf@PLT
	pop %rbp
EndBlock0:
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
