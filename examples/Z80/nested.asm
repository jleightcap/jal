main:
    ; expression tree for nested expression 
    ; (+ (+ 8 5) (- 3 1)):
    ;
    ;      +
    ;     / \
    ;    /   \
    ;   +     -
    ;  / \   / \
    ; 8   5 3   1
    ;
    ; traverse tree depth-first left-to-right,
    ; accumulating result in A register
    ; 8 -> 5 -> 3 -> 1

    ld A, 0
    add A, 8
    add A, 5
    add A, 3
    sub 1
    ret
