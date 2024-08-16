.int_to_ascii:
    xor %r10, %r10
    test %rax, %rax
    jns .convert_positive
    movq $1, %r10 # move if negative
    neg %rax

.convert_positive:
    movq $10, %rbx # divisor; divided is in %rax.
    xor %rdx, %rdx
    div %rbx
    add $48, %dl # convert to ascii
    movb %dl, (%rcx) # store ascii char
    inc %rsi
    dec %rcx # update char ptr
    test %rax, %rax
    jnz .convert_positive
    test %r10, %r10
    jz .end_itoa
    movb $'-', (%rcx)
    dec %rcx
    inc %rsi
.end_itoa:
    inc %rcx # finalize char ptr
    mov %rsi, %rdx # used for syscall conventions
    mov %rcx, %rsi
    ret