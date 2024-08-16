# assume ptr to str is in %rdi
print_str:
    mov %rdi, %rsi
    call strlen
    mov %rax, %rdx
    mov %rsi, %r8
    addq %rdx, %r8
    movb $0x0A, (%r8)
    inc %rdx
    movq $1, %rax
    movq $1, %rdi
    syscall
    movb $0, (%r8)
    ret