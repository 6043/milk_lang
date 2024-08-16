ascii_to_int:
    movzbq (%rdx), %rax
    sub $48, %rax
    mul %rbx
    add %rax, %r8
    mov %rbx, %rax
    mov $10, %rbx
    mul %rbx
    mov %rax, %rbx
    dec %rcx
    test %rcx, %rcx
    jnz ascii_to_int
    inc %rcx
    mov %r8, %rax
    ret