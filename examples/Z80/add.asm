main:
    ld B, $28 ; result of arg1
    ld C, $2  ; result of arg2
    push BC
    ; ...
    ; push and pop equal number of times
    ; ...
    pop BC
    ld A, B
    add A, C
    ret
