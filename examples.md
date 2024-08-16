```
fib.milk: calculates the first n fibonacci numbers and prints them!
```
```c
int! cheese = input
int... cow[10]

cow[0] = 1
cow[1] = 1

for int! cream = 2; cream < cheese; cream = cream + 1 do {
    cow[cream] = cow[cream - 1] + cow[cream - 2]
}

for int! butter = 0; butter < cheese; butter = butter + 1 do {
    moo cow[butter]
}
```
```
$ ./leather fib.milk
[leather] successfully compiled:
    fib.milk -> fib.s
```
```
$ gcc -c -g fib.s -o fib.o
$ ld fib.o -o fib
$ ./fib
```
```
stdout:
10
1
1
2
3
5
8
13
21
34
55
```
<br>

```
prime.milk: calculates all prime numbers up to n!
```

```
int! cow = input
int! cattle = 0

while cattle < cow do {
    int! cream = 2
    int! cheese = cream ** 2
    while cheese < cattle do {
        cream = cream + 1
        cheese = cream ** 2
    }
    int! yogurt = 2
    int! whey = true
    while yogurt < cream do {
        int! butter = cattle % yogurt
        if butter == 0 then {
            whey = false
            break
        }
        yogurt = yogurt + 1
    }
    if whey == true then {
        moo cattle
    }
    cattle = cattle + 1
}
```
```
$ ./leather prime.milk
[leather] successfully compiled:
    prime.milk -> prime.s
```
```
$ gcc -c -g prime.s -o prime.o
$ ld prime.o -o prime
$ ./prime
```
```
stdout:
100
0
1
2
3
4
5
7
9
11
13
17
19
23
25
29
31
37
41
43
47
49
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
