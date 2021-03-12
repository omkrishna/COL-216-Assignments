# A demonstration of some simple MIPS instructions
# used to test QtSPIM

	# Declare main as a global function
	.globl main 

	.text 		

main:
	lw $t3, 100($2)	
	addi $t2, $0, 26
	addi $t3, $0, 26
	slt $t1, $t3, $t2
	beq $t2, $t3, 3
	add $t4, $t2, $t3	
	sub $t5, $t2, $t3	
	sw $t5, Z	
	addi $t6, $0, 100

	li $v0, 10 # Sets $v0 to "10" to select exit syscall
	syscall # Exit

	.data

value:	.word 12
Z:	.word 10