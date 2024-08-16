# assume HEX representation is in %xmm0.
# also assume that the char ptr at the LAST char is in %rcx.
.double_to_ascii:
    xor %r12, %r12
    # save whole number part in r9.
    cvttsd2si %xmm0, %r9
    test %r9, %r9
    jns .convert_positive_dta:
    movq $1, %r12
    movabs $0x8000000000000000, %rax
    movq %rax, %xmm2
    xorpd %xmm2, %xmm0
    cvttsd2si %xmm0, %r9

.convert_positive_dta:
    # get fractional part
    movq $10000000000, %rax
    cvtsi2sd %rax, %xmm1
    mulsd %xmm1, %xmm0
    cvttsd2si %xmm0, %rax
    movq $10000000000, %r8
    xor %rdx, %rdx
    div %r8
    mov %rdx, %rax
    # remove extra zeros
.double_to_ascii_loop:
    mov %rax, %rbx
    test %rax, %rax
    jz .double_to_ascii_loop_end
    xor %rdx, %rdx
    movq $10, %r8
    div %r8
    test %rdx, %rdx
    jz .double_to_ascii_loop # we had no remainder
.double_to_ascii_loop_end:

    mov %rbx, %rax # get good fraction back

    call int_to_ascii
    mov %rsi, %rcx
    mov %rdx, %rsi
    dec %rcx
    movb $46, (%rcx)
    inc %rsi
    dec %rcx

    mov %r9, %rax
    call int_to_ascii

    test %r12, %r12
    jz .end_dtoa
    dec %rsi
    movb $'-', (%rsi)
    inc %rdx
.end_dtoa:
    ret

