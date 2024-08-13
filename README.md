# The Milk Language

### The programming language where only milk and cow-related variables are allowed... (.milk)
### Compiles code into x86-64 AT&T syntax Assembly language, which then can be assembled, linked, and executed on Linux computers.

# Installation/Setup can be found [here](https://github.com/6043/milk_lang/blob/main/installation.md).

# Example: 
```
sqrt_newtons_method.milk
```
Uses Newton's Method to continuously calculate the square root of a number to 10 decimal points until the user enters 0, based on this equation:

```math
$$x_{n+1} = \frac{1}{2}\left(x _{n} + \frac{a}{x_{n}} \right)$$
```
<br/>

```c
int! cow = input

while cow != 0 do {

    real! cheese = 1.0
    real! cream

    int! udder = 0
    real! yogurt

    while udder < 20 do {
        yogurt = cow / cheese
        cream = cheese + yogurt
        cream = cream / 2
        cheese = cream
        udder = udder + 1
    }

    moo cheese

    cow = input
}
```
```
$ ./leather sqrt_newtons_method.milk
[leather] successfully compiled:
    sqrt_newtons_method.milk -> sqrt_newtons_method.s
```

```assembly
.section .text
.globl _start
_start:
   mov %rsp, %rbp
   sub $40, %rsp
   sub $64, %rsp
   mov %rbp, %rsi
   addq $40, %rsi
   movq $0, %rax
   movq $0, %rdi
   movq $64, %rdx
   syscall
   mov %rax, %rcx
   subq $1, %rcx
   mov %rsi, %r9
   addq %rcx, %r9
   subq $1, %r9
   movq $1, %rbx
   xor %r8, %r8
   call ascii_to_int
   movq %rax, -8(%rbp)
.STARTWHILE0:
   movq -8(%rbp), %rax
   mov %rax, %rcx
   movq $0, %rax
   cmpq %rcx, %rax
   setne %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDWHILE0
   movabs $0x3FF0000000000000, %rax
   movq %rax, %xmm0
   movsd %xmm0, -16(%rbp)
   movabs $0x0, %rax
   movq %rax, %xmm0
   movsd %xmm0, -24(%rbp)
   movq $0, %rax
   movq %rax, -32(%rbp)
   movabs $0x0, %rax
   movq %rax, %xmm0
   movsd %xmm0, -40(%rbp)
.STARTWHILE1:
   movq -32(%rbp), %rax
   mov %rax, %rcx
   movq $20, %rax
   cmpq %rax, %rcx
   setl %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDWHILE1
   movsd -16(%rbp), %xmm0
   movsd %xmm0, %xmm1
   movq -8(%rbp), %rax
   cvtsi2sd %rax, %xmm0
   divsd %xmm1, %xmm0
   movsd %xmm0, -40(%rbp)
   movsd -16(%rbp), %xmm0
   movsd %xmm0, %xmm1
   movsd -40(%rbp), %xmm0
   addsd %xmm1, %xmm0
   movsd %xmm0, -24(%rbp)
   movq $2, %rax
   cvtsi2sd %rax, %xmm0
   movsd %xmm0, %xmm1
   movsd -24(%rbp), %xmm0
   divsd %xmm1, %xmm0
   movsd %xmm0, -24(%rbp)
   movsd -24(%rbp), %xmm0
   movsd %xmm0, -16(%rbp)
   movq -32(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   addq %rcx, %rax
   movq %rax, -32(%rbp)
   jmp .STARTWHILE1
.ENDWHILE1:
   movsd -16(%rbp), %xmm0
   movq $1, %rsi
   mov %rbp, %rcx
   addq $103, %rcx
   movq $0x0A, (%rcx)
   dec %rcx
   call double_to_ascii
   movq $1, %rax
   movq $1, %rdi
   syscall
   mov %rbp, %rsi
   addq $40, %rsi
   movq $0, %rax
   movq $0, %rdi
   movq $64, %rdx
   syscall
   mov %rax, %rcx
   subq $1, %rcx
   mov %rsi, %r9
   addq %rcx, %r9
   subq $1, %r9
   movq $1, %rbx
   xor %r8, %r8
   call ascii_to_int
   movq %rax, -8(%rbp)
   jmp .STARTWHILE0
.ENDWHILE0:
.exit:
   movq $60, %rax
   xor %rdi, %rdi
   syscall

int_to_ascii:
   movq $10, %rbx
   xor %rdx, %rdx
   div %rbx
   add $48, %dl
   movb %dl, (%rcx)
   inc %rsi
   dec %rcx
   test %rax, %rax
   jnz int_to_ascii
   inc %rcx
   mov %rsi, %rdx
   mov %rcx, %rsi
   ret

ascii_to_int:
   movb (%r9), %al
   movzbq %al, %rax
   sub $48, %rax
   mul %rbx
   add %rax, %r8
   mov %rbx, %rax
   mov $10, %rbx
   mul %rbx
   mov %rax, %rbx
   dec %rcx
   dec %r9
   test %rcx, %rcx
   jnz ascii_to_int
   inc %r9
   mov %r8, %rax
   ret

exp:
   test %rbx, %rbx
   jz .endexp
   mul %rcx
   dec %rbx
   jmp exp
.endexp:
   ret

double_to_ascii:
   movsd %xmm0, %xmm1
   cvttsd2si %xmm1, %r9
   movq $10000000000, %rax
   cvtsi2sd %rax, %xmm1
   mulsd %xmm1, %xmm0
   cvttsd2si %xmm0, %rax
   movq $10000000000, %r8
   xor %rdx, %rdx
   div %r8
   mov %rdx, %rax
.double_to_ascii_loop:
   mov %rax, %rbx
   test %rax, %rax
   jz .double_to_ascii_loop_end
   xor %rdx, %rdx
   movq $10, %r8
   div %r8
   test %rdx, %rdx
   jz .double_to_ascii_loop
.double_to_ascii_loop_end:
   mov %rbx, %rax
   call int_to_ascii
   mov %rsi, %rcx
   mov %rdx, %rsi
   dec %rcx
   movb $46, (%rcx)
   inc %rsi
   dec %rcx
   mov %r9, %rax
   call int_to_ascii
   ret
```

```
$ gcc -c -g sqrt_newtons_method.s -o sqrt_newtons_method.o
$ ld sqrt_newtons_method.o -o sqrt_newtons_method
$ ./sqrt_newtons_method
```

```
stdout:
100
10.0
64
8.0
279
16.7032930884
1000
31.6227766016
420
20.4939015319
2024
44.9888875168
2025
45.0
0
```

```
prime.milk: finds all prime numbers up to the given input.
```
```c
int! cow = input
int! udder = 2

while udder < cow do {
    int! cheese = 2
    int! cream = cheese ** 2
    while ! cream > udder do {
        cheese = cheese + 1
        cream = cheese ** 2
    }
    int! yogurt = 2
    int! calf = true
    while yogurt < cheese do {
        int! cottagecheese = udder % yogurt
        if cottagecheese == 0 then {
            calf = false
            break
        }
        yogurt = yogurt + 1
    }
    if calf == true then {
        moo udder
    }
    udder = udder + 1
}
```

```
$ ./leather prime.milk
[leather] successfully compiled:
    prime.milk -> prime.s
```

```assembly
.section .text
.globl _start
_start:
   mov %rsp, %rbp
   sub $56, %rsp
   sub $64, %rsp
   mov %rbp, %rsi
   addq $56, %rsi
   movq $0, %rax
   movq $0, %rdi
   movq $64, %rdx
   syscall
   mov %rax, %rcx
   subq $1, %rcx
   mov %rsi, %r9
   addq %rcx, %r9
   subq $1, %r9
   movq $1, %rbx
   xor %r8, %r8
   call ascii_to_int
   movq %rax, -8(%rbp)
   movq $2, %rax
   movq %rax, -16(%rbp)
.STARTWHILE0:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq -8(%rbp), %rax
   cmpq %rax, %rcx
   setl %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDWHILE0
   movq $2, %rax
   movq %rax, -24(%rbp)
   movq -24(%rbp), %rax
   mov %rax, %rcx
   movq $2, %rax
   mov %rax, %rbx
   movq $1, %rax
   call exp
   movq %rax, -32(%rbp)
.STARTWHILE1:
   movq -32(%rbp), %rax
   mov %rax, %rcx
   movq -16(%rbp), %rax
   cmpq %rax, %rcx
   setg %al
   movzbq %al, %rax
   xor $1, %rax
   test %rax, %rax
   jz .ENDWHILE1
   movq -24(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   addq %rcx, %rax
   movq %rax, -24(%rbp)
   movq -24(%rbp), %rax
   mov %rax, %rcx
   movq $2, %rax
   mov %rax, %rbx
   movq $1, %rax
   call exp
   movq %rax, -32(%rbp)
   jmp .STARTWHILE1
.ENDWHILE1:
   movq $2, %rax
   movq %rax, -40(%rbp)
   movq $1, %rax
   movq %rax, -48(%rbp)
.STARTWHILE2:
   movq -40(%rbp), %rax
   mov %rax, %rcx
   movq -24(%rbp), %rax
   cmpq %rax, %rcx
   setl %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDWHILE2
   movq -40(%rbp), %rax
   mov %rax, %rcx
   movq -16(%rbp), %rax
   xor %rdx, %rdx
   div %rcx
   mov %rdx, %rax
   movq %rax, -56(%rbp)
   movq -56(%rbp), %rax
   mov %rax, %rcx
   movq $0, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF0
   movq $0, %rax
   movq %rax, -48(%rbp)
   jmp .ENDWHILE2
.ENDIF0:
   movq -40(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   addq %rcx, %rax
   movq %rax, -40(%rbp)
   jmp .STARTWHILE2
.ENDWHILE2:
   movq -48(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF1
   movq -16(%rbp), %rax
   movq $1, %rsi
   mov %rbp, %rcx
   addq $119, %rcx
   movq $0x0A, (%rcx)
   dec %rcx
   call int_to_ascii
   movq $1, %rax
   movq $1, %rdi
   syscall
.ENDIF1:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   addq %rcx, %rax
   movq %rax, -16(%rbp)
   jmp .STARTWHILE0
.ENDWHILE0:
.exit:
   movq $60, %rax
   xor %rdi, %rdi
   syscall

int_to_ascii:
   movq $10, %rbx
   xor %rdx, %rdx
   div %rbx
   add $48, %dl
   movb %dl, (%rcx)
   inc %rsi
   dec %rcx
   test %rax, %rax
   jnz int_to_ascii
   inc %rcx
   mov %rsi, %rdx
   mov %rcx, %rsi
   ret

ascii_to_int:
   movb (%r9), %al
   movzbq %al, %rax
   sub $48, %rax
   mul %rbx
   add %rax, %r8
   mov %rbx, %rax
   mov $10, %rbx
   mul %rbx
   mov %rax, %rbx
   dec %rcx
   dec %r9
   test %rcx, %rcx
   jnz ascii_to_int
   inc %r9
   mov %r8, %rax
   ret

exp:
   test %rbx, %rbx
   jz .endexp
   mul %rcx
   dec %rbx
   jmp exp
.endexp:
   ret

double_to_ascii:
   movsd %xmm0, %xmm1
   cvttsd2si %xmm1, %r9
   movq $10000000000, %rax
   cvtsi2sd %rax, %xmm1
   mulsd %xmm1, %xmm0
   cvttsd2si %xmm0, %rax
   movq $10000000000, %r8
   xor %rdx, %rdx
   div %r8
   mov %rdx, %rax
.double_to_ascii_loop:
   mov %rax, %rbx
   test %rax, %rax
   jz .double_to_ascii_loop_end
   xor %rdx, %rdx
   movq $10, %r8
   div %r8
   test %rdx, %rdx
   jz .double_to_ascii_loop
.double_to_ascii_loop_end:
   mov %rbx, %rax
   call int_to_ascii
   mov %rsi, %rcx
   mov %rdx, %rsi
   dec %rcx
   movb $46, (%rcx)
   inc %rsi
   dec %rcx
   mov %r9, %rax
   call int_to_ascii
   ret

```

```
$ gcc -c prime.s -o prime.o
$ ld prime.o -o prime
$ ./prime
```

```
stdout:
100
2
3
5
7
11
13
17
19
23
29
31
37
41
43
47
53
59
61
67
71
73
79
83
89
97
```

### What happens if you try to compile a file which doesn't use cow or milk-related variabls? Just watch...

```
here is a binary search program to find the sqrt of a number, without cow-related variables:
```

```c
int! n = input
while n != 0 do {
    int! l = 0
    int! r = n

    while ! l > r do {
        int! mid = l + r
        mid = mid / 2
        int! squared = mid ** 2
        if squared == n then {
            break
        }
        if squared > n then {
            r = mid - 1
        }
        if squared < n then {
            l = mid + 1
        }
    }

    if squared > n then {
        mid = mid - 1
    }

    moo mid
    n = input
}
```

```
$ ./leather sqrt.milk
[leather] compilation failed:
    variable name 'n' is not MILK enough! find a list of acceptable variable names at https://github.com/6043/milk_lang/blob/main/allowed_vars.txt
```

# Have fun coding with this somewhat obfuscated language !!! And good luck to other participants
