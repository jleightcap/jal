ROM_START       .equ $8000

  .org ROM_START
        lda #$20
        brk
