			# This code was produced by the CERI Compiler
	.data
	.align 8
b:	.quad 0
c:	.quad 0
z:	.quad 0
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $6
	pop b
	push $2
	pop z
	push z
	push b
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	jb Vrai1	# If below
	push $0		# False
	jmp Suite1
Vrai1:	push $0xFFFFFFFFFFFFFFFF		# True
Suite1:
IfBlock0:
	pop %rax
	cmpq $0, %rax
	je ElseBlock0
	push $28
	pop c
	jmp FinBlock0
ElseBlock0:
FinBlock0:
	push z
	push $2
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	je Vrai2	# If equal
	push $0		# False
	jmp Suite2
Vrai2:	push $0xFFFFFFFFFFFFFFFF		# True
Suite2:
IfBlock1:
	pop %rax
	cmpq $0, %rax
	je ElseBlock1
	push $22
	pop c
	jmp FinBlock1
ElseBlock1:
	push $2
	pop c
FinBlock1:
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
