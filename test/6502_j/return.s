    org $8000

; for 6502, assume that the return value of
; main must be in A
start:
    lda #42
    brk

