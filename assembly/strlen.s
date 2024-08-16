strlen:
    # assume ptr is in %rdi
    xor %r9, %r9
.strlen_loop:
    movb (%rdi), %al
    test %al, %al
    jz .end_strlen
    inc %r9
    inc %rdi
    jmp .strlen_loop
.end_strlen:
    mov %r9, %rax
    ret