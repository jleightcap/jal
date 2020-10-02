    .globl _start
_start:
    addi    t0, x0, 126
    addi    t1, x0, 3
    div     a0, t0, t1
    addi    a7, x0, 93
    ecall
