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
	
	add $t4, $t2, $t3
	j endblock
	sub $t5, $t2, $t3	
	
	sw $t5, Z	
	addi $t6, $0, 100

	beq $t2, $t3, endblock

endblock:
	add $t4, $t2, $t3

.data
value:	.word 12
Z:	.word 10