# Assignment 1 - COL216, Semester 2 2020-21
# MIPS Assembly Program for obtaining the area under a curve formed by joining
# successive points by a straight line.
# The program uses 32 bit registers for multiplication and might behave incorrectly for extremely large numbers (overflow)

# Authors : Harshita (2019CS10357) & Om Krishna (2019CS10272)

# The C/C++/Java-like equivalent of this program is 
# s1,s2 - x1,y1 ;; t1,t2 - x2,y2
# for (int i=1; i < s0; i++){
#    float t1, t2;
#    cin >> t1 >> t2
#    if (s2*t2 > 0) 
#    s3 += float(((t1-s1)*(abs(t2+s2)))/s4)
#    else s3 += ((t1-s1)*(t2*t2 + s2*s2))/(s4*abs(t2-s2))
#    s1 = t1
#    s2 = t2
#}


    .data 
msg: .asciiz "Inputs\nn : "
msg2: .asciiz "(X,Y) co­ordinates sorted according to X co­ordinate: (in order - x1,y1,x2,y2...) : \n"
msg3: .asciiz "Area = "
error_msg: .asciiz "Err: Unsuitable input value"

    .text
main:
    li $v0,4
    la $a0,msg
    syscall

    li $v0,5
    syscall
    move $s0,$v0 # s0 contains number of co-ordinates

    blt $s0,2,error_input

    li $v0,4
    la $a0,msg2
    syscall

    li $v0,5
    syscall
    move $s1,$v0 # s1 stores x1

    li $v0,5
    syscall
    move $s2,$v0 # s2 stores y1

    
    li.s $f0,0.0 # f0 stores the area (floating point register)
    li.s $f1,2.0 # f1 stores the constant 2.0 
    li $t0,1 # loop variable
    

case1:
    beq $t0,$s0, end # if t0 (loop variable) == s0 (no of co-ordinates) ---> go to end

    li $v0,5
    syscall
    move $s5,$v0 # s5 stores x2

    blt $s5,$s1,error_input

    li $v0,5
    syscall
    move $s6,$v0 # s6 stores y2

    #s1-x1;  s2-y1
    #s5-x2;  s6-y2

    mul $t1,$s2,$s6 # t1 = y1*y2

    blt $t1,$0, case2 # t1<0 => one of y1 and y2 is negative and other is positive. Area is calculated in case2 then 

    # CASE 1 - AREA IS CALCULATED USING TRAPEZIUM RULE (when y1,y2 are both positive or both negative)
    # area = (x2-x1)|y2-y1| / 2

    sub $t2,$s5,$s1 # t2 = x2 - x1
    add $t3,$s6,$s2 # t3 = y2 - y1
    abs $t3,$t3     # t3 = |y2 - y1|
    mul $t4,$t2,$t3 # t4 = (x2-x1)*|y2-y1|
     
    mtc1 $t4, $f2   # convert from integer to float
    cvt.s.w $f2, $f2 

    div.s $f2,$f2,$f1 # f2 = (x2-x1)*|y2-y1| / 2 (stores as floating point number)
    add.s $f0,$f0,$f2 # add to existing area

    j endloop 


case2:
    #s1-x1;  s2-y1
    #s5-x2;  s6-y2

    # CASE 2 - one of y1 and y2 is positive and one is negative
    # area = (x2-x1)*(y2^2+y1^2) / 2 |y2-y1|       * SEE WRITE-UP FOR DERIVATION *

    sub $t1,$s5,$s1 # t1 = x2-x1
    mul $t2,$s2,$s2 # t2 = y1^2
    mul $t3,$s6,$s6 # t3 = y2^2
    add $t4,$t2,$t3 # t4 = y1^2 + y2^2
    mul $t5,$t1,$t4 # t5 = (x2-x1)(y1^2 + y2^2)

    mtc1 $t5, $f2 
    cvt.s.w $f2, $f2

    div.s $f2,$f2,$f1 # t5 = t5/2
    sub $t6,$s6,$s2 # t6 = y2-y1
    abs $t6,$t6
    
    mtc1 $t6, $f3
    cvt.s.w $f3, $f3

    div.s $f2,$f2,$f3 # t5 = (x2-x1)(y1^2 + y2^2)/(2*|y2-y1|)
    add.s $f0,$f0,$f2

    j endloop

endloop:
    move $s1,$s5 
    move $s2,$s6

    addi $t0,$t0,1 # increment the loop counter by 1
    j case1

end:
    li $v0,4
    la $a0,msg3
    syscall

    li $v0,2
    mov.s $f12,$f0 # f0 stores the area
    syscall

    li $v0,10
    syscall

error_input:
    li $v0,4
    la $a0, error_msg
    syscall

    li $v0,10
    syscall