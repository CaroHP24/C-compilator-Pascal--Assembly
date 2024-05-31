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
t:	.space 500
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
DebutBlock0:
	push $12
	pop %rax
	movl %eax, t+4
	push $16
	pop %rax
	movl %eax, t+8
	push $7
	pop %rax
	movl %eax, t+12
	push $13
	pop %rax
	movl %eax, t+16
	push $9
	pop %rax
	movl %eax, t+20
	push t
movq t, %rax
movl (%rax), %ebx 
EndBlock0:
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
