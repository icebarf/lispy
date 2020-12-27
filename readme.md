# BYOLISP_A

## Project Info

**This project is a work in progress LISP**
This lisp is not meant for programming purposes but rather is a fun project of mine. I decided to write this as I wanted to learn C and do a fun project as well,

The code used in this project is written by [Daniel Holden](https://github.com/orangeduck) which is present [on this webpage](https://github.com/orangeduck/BuildYourOwnLisp), this book has been very insightful and fun to read and I am enjoying reading it.

Although you're free to use to this code is available under CC License, I am putting it under MIT License as it contains modifications and bonus project question code which is not written by Daniel Holden, anyway the project is in early stange and I am going to continue working on it slowly as I continue to explore the world of C.

I have also provided compiled binaries for windows and linux as well So that you can try what I have currently done

> Note: I will fully release when I finish reading the book

## Compile Instructions

**Pre-requisites**
> must have any C compiler e.g GNU/GCC, Clang

**Compilation**
Place main.c mpc.c mpc.h in a single folder 

or

```git clone https://github.com/sysgrammar/byolisp_a.git

cd byolisp_a```
    
**Windows**

```gcc -std=c99 -Wall main.c mpc.c -lm -o byolisp_a.-early-0.2```

**GNU/Linux**

```gcc -std=c99 -Wall main.c mpc.c -ledit -lm -o byolisp_a.-early-0.2```

**NOTE(Linux users only)**

Editline could be missing from your distro and it may not compile, therefore you need to do this

>This command is for apt package manager 

```sudo apt-get install liibedit-dev```

please note that arch users will have to modify the `main.c` file as history.c and readline.c have their names changed and are in separate locations.

### Current Progress

- Display the structure of input
- Shows what type of input was entered

### Goals

- Learn C
- Learning how a language is made
- Learning to write good code