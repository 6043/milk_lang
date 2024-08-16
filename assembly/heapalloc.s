heapalloc:
    # assume %rdi stores number of bytes to allocate
    mov %rdi, %rsi
    movq $0, %rdi
    movq $0x3, %rdx
    movq $0x22, %r10
    movq $-1, %r8
    xor %r9, %r9
    movq $9, %rax
    syscall
    ret