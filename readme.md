
# Project Info

**This project is a work in progress LISP**
This lisp is not meant for programming purposes but rather is a fun project of mine. I decided to write this as I wanted to learn C and do a fun project as well,

The book being used for learning and this project is [Build Your Own LISP by Daniel Holden](http://buildyourownlisp.com/)

The book used in this project is written by [Daniel Holden](https://github.com/orangeduck), this book has been very insightful and fun to read and have taught me many things.
I do recommend giving it a read even if you are not interested in writing a full lisp.

I have also provided compiled binaries for windows and linux as well.
**[Download](https://github.com/sysgrammar/byolisp_a/releases)**

> Note: A full 1.0 release will be out when I, the dev finishes reading the book.

## Changelog (Begun since version early-0.4)
**Version `early-0.4`**
- Added error handling to the LISP
- Added two new opeartors: `min` and `max`
- Improved code readability
- Some cleanup and minor changes

**Version `early-0.5`**
- Added support for decimal input
- Added decimal calculations
- Added Symbolic Expressions like in a Typical Lisp
- Refactored Code

**Version `early-0.6`**
- Added Quoted Expressions
- Added new functions for manipulation of Q-Expressions
- Added macros for type and count checking for Q-Expression
- Evaluation support for q-expressions

### Current Progress

- Get mathematical input in polish notation
- Has error handling

**Can perform the following mathematical opeartions:**
- Addition
- Subtraction
- Multiplication
- Division & remainder(modulus)
- Power
- Min/Max

**Quoted Expression functions:**
- list
- head
- tail
- join
- eval
- cons
- len
- init

## Compile Instructions

**Pre-requisites**
> must have any C compiler e.g GNU/GCC, Clang

**Compilation**

Place main.c mpc.c mpc.h in a single folder 

or

```git clone https://github.com/sysgrammar/byolisp_a.git```

```cd byolisp_a```
    
**Windows**

```gcc -std=c99 -Wall main.c mpc.c -lm -o byolisp_a```

**GNU/Linux**

```gcc -std=c99 -Wall main.c mpc.c -ledit -lm -o byolisp_a```

**NOTE(Linux users only)**

Editline could be missing from your distro and it may not compile, therefore you need to do this

>This command is for apt package manager 

```sudo apt-get install liibedit-dev```

Please note that arch users will have to modify the `main.c` file as history.c and readline.c have their names changed and are in separate locations.

**Termux (Mobile testing using CLang)**
Remove ```#include <editline/history.h>``` from `main.c`.

Then compile using:
```clang -std=c99 -Wall main.c mpc.c -ledit -lm -o byolisp_a```

### Syntax

#### Mathematical Operations:

**Addition**

To `add` two numbers
```byolisp_a> + 5 6```

To add more than two numbers
```byolisp_a> + 5 6 7 32 543```

**Subtraction**

To `sub` two numbers
```byolisp_a> - 5 6```

To sub more than two numbers
```byolisp_a> - 543 32 7 5```

**Multiplication**

To `mul` two numbers
```byolisp_a> * 5 6```

To mul more than two numbers
```byolisp_a> * 5 6 7 32 543```

**Division(Quotient)**

To `div` two numbers
```byolisp_a> / 30 5```

**Remainder**

To find the `rem` of two numbers
```byolisp_a> % 30 5```

**Power**

To find the `pow` of two numbers(Input base first and then the power to the base should be raised):
```byolisp_a> ^ 5 6```

**Minimum**
To return the `min` of two or more numbers entered:
```byolisp_a> min 4 39```

**Maximum**
To return the `max` of two or more numbers entered:
```byolisp_a> max 4 39```

#### Quoted Expressions:

**List**
Returns a quoted expression/list of normal numbers:
```byolisp_a> list 1 2 3 4```

**Head**
Returns first element of the q-expression/list:
```byolisp_a> head {1 2 3 4}```

**Tail**
Returns the q-expression with the first element removed from it:
```byolisp_a> tail {1 2 3 4}```

**Join**
Takes two q-expressions/lists as input and joins them:
```byolisp_a> join {1 2 3} {4 5 6}```

**Eval**
Takes a Q-Expression and evaluates it as if it were a S-Expression:
```byolisp_a> eval (tail {tail tail {5 6 7}})```

**Cons**
Takes a single value and a q-expression and appends the value to the front of q-expression:
```byolisp_a> cons 1 {2 3}```

**len**
Returns the lenth of the list/q-expression:
```byolisp_a> len {2 34 5 68}```

**init**
Returns the q-expression with last element removed from it:
```byolisp_a> init {3 3 5 6 7}```


### Goals

- Learn C
- Learning how a language is made
- Learning to write good code
- Complete this project.