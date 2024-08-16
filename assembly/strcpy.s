strcpy:
    # assume src ptr is %rdi
    # assume dest ptr is %rsi
    # assume num bytes is in %rdx
    test %rdx, %rdx
    jz .end_strcpy
    movb (%rdi), %al
    movb %al, (%rsi)
    dec %rdx
    inc %rdi
    inc %rsi
    jmp strcpy
.end_strcpy:
    movb $0, (%rsi) # null character
    ret