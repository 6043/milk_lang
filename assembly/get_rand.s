get_rand:
    # assume n in %rcx.
    movq $318, %rax
    movq $8, %rsi
    movq $0, %rdx
    movq $318, %rax
    syscall
    movq (%rdi), %rax
    ret