.data
integer: .word 50

.text
main:
	addi $t1, $0, 100
	addi $t2, $0, 20

	lw $t3, addr
	lw $t4, $t3
	

	li $v0, 10 
	syscall

