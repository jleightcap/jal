    .globl _start
_start:
    addi    t0, x0, 21
    addi    t1, x0, 2
    mul     a0, t0, t1
    addi    a7, x0, 93
    ecall
