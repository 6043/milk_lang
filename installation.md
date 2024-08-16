# Step 1
### Get running on Linux.
You can either do this by running a Linux OS, or, alternatively, if on Windows, by using WSL:

To install WSL, open Powershell as an administrator and run
```
wsl --install
```

If prompted, I would recommend installing Ubuntu.

Once this finishes installing, you can run WSL from the start menu and to access files on your Windows C drive, for example, you can use: 

```
cd /mnt/c/{path}
```

# Step 2
### Install gcc for assembling/linking.

Open WSL or a Linux terminal and type the commands

```
sudo apt-get update
sudo apt install build-essential
```

Running this will confirm you have successfully installed it:
```
gcc --version
```

# Step 4
### Installing Leather (the compiler)

Download it [here](https://github.com/6043/milk_lang/releases/download/v1/leather). That's it.

# Step 3
### Writing a program

Using the syntax found [here](https://github.com/6043/milk_lang/blob/main/syntax.txt) and allowed variables found [here](https://github.com/6043/milk_lang/blob/main/allowed_vars.txt), code a program! Make sure to add .milk as the file extension.

As an example, lets use prime.milk, here is the code:

```
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

# Step 4
### Compile the program

In the terminal, run:

```
./leather {filename.milk}
```

Which, in our case, will be:

```
./leather prime.milk
```

You should get the following message if there are no errors with your code:

```
[leather] successfully compiled:
    prime.milk -> prime.s
```

# Step 5 
### Assembling and Linking

We will use gcc for this part.

Run the commands 

```
gcc -c {filename}.s -o {filename}.o
ld {filename}.o -o {filename}
```

Which, in our case, will be:

```
gcc -c prime.s -o prime.o
ld prime.o -o prime
```

# Step 6
### Run your program!

Now, finally, we can simply run the program with

```
./filename
```

or, in our case:

```
./prime
```

which produces this output once we enter "100":

```
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

