	.globl	_start

_start:
        addi    x1,x0,44
        addi    x2,x0,2
        sub     a0,x1,x2        # a0 = x1 - x2 (return value)
        addi    a7,x0,93        # a7 = 0 + 93  (syscall exit)
        ecall                   # syscall(93)
