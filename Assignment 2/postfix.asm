.data
    prompt: .asciiz "Enter the postfix expression : \n"
    err_str: .asciiz "Error Input"
    str: .space 256

.text

main:   
    li $v0,4
    la $a0,prompt
    syscall

    li $v0,8
    la $a0,str
    li $a1, 256
    syscall

    la $s0, str     #s0 will hold string that will be iterated through

    li $s1, '+'
    li $s2, '-'
    li $s3, '*'
    li $s4, '0'
    li $s5, '9'
    li $s6, '\n'
    li $s7, ' '
    li $t0,0 # store stack size

    loop:
        lb $t1, 0($s0)       
        beq $t1, $zero, end
        add $s0,1

        beq $t1,$s1,addBlock
        beq $t1,$s2,subBlock
        beq $t1,$s3,mulBlock
 
        beq $t1,$s6, loop
        beq $t1,$s7, loop

        blt $t1,$s4, errorInput
        bgt $t1,$s5, errorInput

        j digitBlock

        #li $v0,11
        #la $a0,0($t1)
        #syscall

        j loop
        
    addBlock:
        blt $t0,2,errorInput
        lw $t2, ($sp)
        add $sp, $sp, 4

        lw $t3, ($sp)
        add $sp, $sp, 4


        add $t0,$t0,-2
        add $t4,$t2,$t3

        subu $sp, $sp, 4
        sw $t4, ($sp)
        add $t0,$t0,1

        #li $v0,4
        #la $a0, add_str
        #syscall
        j loop

    mulBlock:
        blt $t0,2,errorInput
        lw $t2, ($sp)
        add $sp, $sp, 4

        lw $t3, ($sp)
        add $sp, $sp, 4
 
        add $t0,$t0,-2
        mul $t4,$t2,$t3

        subu $sp, $sp, 4
        sw $t4, ($sp)
        add $t0,$t0,1

        #li $v0,4
        #la $a0, add_str
        #syscall
        j loop

    subBlock:
        blt $t0,2,errorInput
        lw $t2, ($sp)
        add $sp, $sp, 4

        lw $t3, ($sp)
        add $sp, $sp, 4
 
        add $t0,$t0,-2
        sub $t4,$t3,$t2

        subu $sp, $sp, 4
        sw $t4, ($sp)
        add $t0,$t0,1

        #li $v0,4
        #la $a0, add_str
        #syscall
        j loop

    digitBlock:
        add $t0,$t0,1
        subu $sp, $sp, 4
        sub $t1,$t1,48
        sw $t1, ($sp)

        #li $v0,4
        #la $a0, digit_str
        #syscall
        j loop

    end:
        bgt $t0,1, errorInput

        lw $t5,($sp)
        li $v0,1
        la $a0,($t5)
        syscall

        li $v0,10
        syscall

    errorInput:
        li $v0,4
        la $a0, err_str
        syscall

        li $v0,10
        syscall