# The Milk Language

### The programming language where only milk and cow-related variables are allowed... (.milk)
### Compiles code into x86-64 AT&T syntax Assembly language, which then can be assembled, linked, and executed on Linux computers.

# Installation/Setup can be found [here](https://github.com/6043/milk_lang/blob/main/installation.md).
# A Demo Video can be found [here](https://youtu.be/Eu3_kIzMgHc).

# Example - more can be found [here](https://github.com/6043/milk_lang/tree/main/examples)
```
rps.milk: Play Rock, Paper, Sciccors against the computer!
```

<br/>

```c
print "Hello. Welcome to Rock, Paper, Scissors!"

int! cheese
int! cream

while cheese != 4 do {
    print " - 1 for Rock"
    print " - 2 for Paper"
    print " - 3 for Scissors"
    print " - 4 to exit"
    
    cheese = input

    if cheese == 4 then {
        break
    }

    cream = random % 3
    cream = cream + 1
    
    if cream == 1 then {
        print "The computer chooses Rock"
    }
    if cream == 2 then {
        print "The computer chooses Paper"
    }
    if cream == 3 then {
        print "The computer chooses Scissors"
    }

    if cheese == 1 then {
        if cream == 1 then {
            print "You tie."
        }
        if cream == 2 then {
            print "You lose!"
        }
        if cream == 3 then {
            print "You win!"
        }
    }

    if cheese == 2 then {
        if cream == 1 then {
            print "You win!"
        }
        if cream == 2 then {
            print "You tie."
        }
        if cream == 3 then {
            print "You lose!"
        }
    }

    if cheese == 3 then {
        if cream == 1 then {
            print "You lose!"
        }
        if cream == 2 then {
            print "You win!"
        }
        if cream == 3 then {
            print "You tie."
        }
    }
}
```
```
$ ./leather rps.milk
[leather] successfully compiled:
    rps.milk -> rps.s
```

```assembly
.section .data
str0:
   .string "Hello. Welcome to Rock, Paper, Scissors!"
str1:
   .string " - 1 for Rock"
str2:
   .string " - 2 for Paper"
str3:
   .string " - 3 for Scissors"
str4:
   .string " - 4 to exit"
str5:
   .string "The computer chooses Rock"
str6:
   .string "The computer chooses Paper"
str7:
   .string "The computer chooses Scissors"
str8:
   .string "You tie."
str9:
   .string "You lose!"
str10:
   .string "You win!"
.section .text
.globl _start
_start:
   mov %rsp, %rbp
   sub $16, %rsp
   sub $128, %rsp
   lea str0(%rip), %rax
   mov %rax, %rdi
   call print_str
   movq $0, %rax
   movq %rax, -8(%rbp)
   movq $0, %rax
   movq %rax, -16(%rbp)
.STARTLOOP0:
   movq -8(%rbp), %rax
   mov %rax, %rcx
   movq $4, %rax
   cmpq %rcx, %rax
   setne %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDLOOP0
   lea str1(%rip), %rax
   mov %rax, %rdi
   call print_str
   lea str2(%rip), %rax
   mov %rax, %rdi
   call print_str
   lea str3(%rip), %rax
   mov %rax, %rdi
   call print_str
   lea str4(%rip), %rax
   mov %rax, %rdi
   call print_str
   mov %rbp, %rsi
   addq $16, %rsi
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
   movq -8(%rbp), %rax
   mov %rax, %rcx
   movq $4, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF0
   jmp .ENDLOOP0
.ENDIF0:
   push %rcx
   movq $3, %rax
   mov %rax, %rcx
   mov %rbp, %rdi
   addq $16, %rdi
   push %rcx
   call get_rand
   pop %rcx
   xor %rdx, %rdx
   div %rcx
   mov %rdx, %rax
   pop %rcx
   movq %rax, -16(%rbp)
   push %rcx
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   addq %rcx, %rax
   pop %rcx
   movq %rax, -16(%rbp)
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF1
   lea str5(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF1:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $2, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF2
   lea str6(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF2:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $3, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF3
   lea str7(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF3:
   movq -8(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF4
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF5
   lea str8(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF5:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $2, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF6
   lea str9(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF6:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $3, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF7
   lea str10(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF7:
.ENDIF4:
   movq -8(%rbp), %rax
   mov %rax, %rcx
   movq $2, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF8
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF9
   lea str10(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF9:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $2, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF10
   lea str8(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF10:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $3, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF11
   lea str9(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF11:
.ENDIF8:
   movq -8(%rbp), %rax
   mov %rax, %rcx
   movq $3, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF12
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $1, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF13
   lea str9(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF13:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $2, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF14
   lea str10(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF14:
   movq -16(%rbp), %rax
   mov %rax, %rcx
   movq $3, %rax
   cmpq %rcx, %rax
   sete %al
   movzbq %al, %rax
   test %rax, %rax
   jz .ENDIF15
   lea str8(%rip), %rax
   mov %rax, %rdi
   call print_str
.ENDIF15:
.ENDIF12:
   jmp .STARTLOOP0
.ENDLOOP0:
.exit:
   movq $60, %rax
   xor %rdi, %rdi
   syscall

int_to_ascii:
   xor %r10, %r10
   test %rax, %rax
   jns .convert_positive
   movq $1, %r10
   neg %rax
.convert_positive:
   movq $10, %rbx
   xor %rdx, %rdx
   div %rbx
   add $48, %dl
   movb %dl, (%rcx)
   inc %rsi
   dec %rcx
   test %rax, %rax
   jnz .convert_positive
   test %r10, %r10
   jz .end_itoa
   movb $'-', (%rcx)
   dec %rcx
   inc %rsi
.end_itoa:
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
   xor %r12, %r12
   cvttsd2si %xmm0, %r9
   test %r9, %r9
   jns .convert_positive_dta
   movq $1, %r12
   movabs $0x8000000000000000, %rax
   movq %rax, %xmm2
   xorpd %xmm2, %xmm0
   cvttsd2si %xmm0, %r9
.convert_positive_dta:
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
   test %r12, %r12
   jz .end_dtoa
   dec %rsi
   movb $'-', (%rsi)
   inc %rdx
.end_dtoa:
   ret

heapalloc:
   mov %rdi, %rsi
   movq $0, %rdi
   movq $0x3, %rdx
   movq $0x22, %r10
   movq $-1, %r8
   xor %r9, %r9
   movq $9, %rax
   syscall
   ret

strcpy:
   test %rdx, %rdx
   jz .end_strcpy
   movb (%rdi), %al
   movb %al, (%rsi)
   dec %rdx
   inc %rdi
   inc %rsi
   jmp strcpy
.end_strcpy:
   movb $0, (%rsi)
   ret

strlen:
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

get_rand:
   movq $318, %rax
   movq $8, %rsi
   movq $0, %rdx
   syscall
   movq (%rdi), %rax
   ret

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
$ gcc -c rps.s -o rps.o
$ ld rps.o -o rps
$ ./rps
```

```
stdout:
Hello. Welcome to Rock, Paper, Scissors!
 - 1 for Rock
 - 2 for Paper
 - 3 for Scissors
 - 4 to exit
1
The computer chooses Scissors
You win!
 - 1 for Rock
 - 2 for Paper
 - 3 for Scissors
 - 4 to exit
1
The computer chooses Rock
You tie.
 - 1 for Rock
 - 2 for Paper
 - 3 for Scissors
 - 4 to exit
2
The computer chooses Scissors
You lose!
 - 1 for Rock
 - 2 for Paper
 - 3 for Scissors
 - 4 to exit
4
```
