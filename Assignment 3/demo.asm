# A demonstration of some simple MIPS instructions
# used to test QtSPIM

	# Declare main as a global function
	.globl main 

	.text 		

main:
	li $t2, 25		
	lw $t3, value	
	addi $t2, $0, 26
	addi $t3, $0, 7
	add $t4, $t2, $t3	
	sub $t5, $t2, $t3	
	sw $t5, Z		


	li $v0, 10 # Sets $v0 to "10" to select exit syscall
	syscall # Exit

	.data

value:	.word 12
Z:	.word 0