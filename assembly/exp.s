exp:
    test %rbx, %rbx
    jz .endexp
    mul %rcx
    dec %rbx
    jmp exp
.endexp:
    ret