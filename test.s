			# This code was produced by the CERI Compiler
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $5
	push $6
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	jb Vrai
Faux:	 push $0 			 #faux
	 jmp FinExp
Vrai:	 push $-1 			 #vrai
FinExp:
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
