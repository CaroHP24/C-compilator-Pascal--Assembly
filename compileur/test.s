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
t:	.quad 0,0,0,0,0
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
EndBlock0:
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
