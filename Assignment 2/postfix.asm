# Assignment 2 - COL216, Semester 2 2020-21
# MIPS Assembly Program for evaluating a postfix expression entered as a string
# The program uses 32 bit registers for multiplication and might behave incorrectly for extremely large numbers (overflow)

# Authors : Harshita (2019CS10357) & Om Krishna (2019CS10272)

# The logic for the program uses the STACK data structure
# If a digit is read, then it is pushed into the stack
# If an operator (+,-,*) is read, then two numbers are popped from the stack, evaluated using the expression, and pushed back
# If the expression is valid, at the end of operations, only 1 element remains in stack -> the value of the postfix expression


.data
    prompt: .asciiz "Enter the postfix expression : \n"
    err_str: .asciiz "Error Input"
    str: .space 256 # We can read an expression of size 256 bytes 

.text

main:   
    li $v0,4
    la $a0,prompt
    syscall

    # Reading a string from console and storing it in str
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
    li $t0,0        #t0 stores stack size

    loop:
        lb $t1, 0($s0)       # read the first character of s0
        beq $t1, $zero, end  # if a zero character is read, then we've read the complete string and we move to the end
        add $s0,1

        beq $t1,$s1,addBlock
        beq $t1,$s2,subBlock
        beq $t1,$s3,mulBlock
 
        beq $t1,$s6, loop
        beq $t1,$s7, loop

        blt $t1,$s4, errorInput
        bgt $t1,$s5, errorInput

        j digitBlock

        
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

        j loop

    digitBlock:
        add $t0,$t0,1
        subu $sp, $sp, 4
        sub $t1,$t1,48
        sw $t1, ($sp)

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