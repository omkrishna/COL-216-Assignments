# A demonstration of some simple MIPS instructions
# used to test QtSPIM

	# Declare main as a global function
	.globl main 

	.text 		

main:
	li $t2, 25		
	lw $t3, value		# Load the word stored in value (see bottom)
	add $t4, $t2, $t3	# Add
	sub $t5, $t2, $t3	# Subtract
	sw $t5, Z		


	li $v0, 10 # Sets $v0 to "10" to select exit syscall
	syscall # Exit

	.data

value:	.word 12
Z:	.word 0