.text
main:
    addi $s0, $s0, 100
    addi $s1, $s1, 0x1A
    add $s2, $s0, $s1
    sub $s3, $s2, $s1
    mul $s4, $s1, $s2

end:
	addi $s7, $zero, 100
    li $v0,10
    syscall

