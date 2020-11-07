	.globl	_start

_start:
        addi    a0,x0,42        # a0 = 0 + 42 (return value)
        addi    a7,x0,93        # a7 = 0 + 93 (syscall exit)
        ecall                   # syscall(93)
