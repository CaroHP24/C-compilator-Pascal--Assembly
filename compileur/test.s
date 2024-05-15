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
	ja Vrai1	# If above
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
	push z
	push $3
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	je Vrai3	# If equal
	push $0		# False
	jmp Suite3
Vrai3:	push $0xFFFFFFFFFFFFFFFF		# True
Suite3:
IfBlock2:
	pop %rax
	cmpq $0, %rax
	je ElseBlock2
	push $22
	pop c
	jmp FinBlock2
ElseBlock2:
	push $65
	pop c
FinBlock2:
EndBlock1:
	jmp FinBlock0
ElseBlock0:
	push $44
	pop c
FinBlock0:
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
