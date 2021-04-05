
# Project Info

**This project is a work in progress LISP**
This lisp is not meant for programming purposes but rather is a fun project of mine. I decided to write this as I wanted to learn C and do a fun project as well,

The book being used for learning and this project is [Build Your Own LISP by Daniel Holden](http://buildyourownlisp.com/)

The book used in this project is written by [Daniel Holden](https://github.com/orangeduck), this book has been very insightful and fun to read and have taught me many things.
I do recommend giving it a read even if you are not interested in writing a full lisp.

I have also provided compiled binaries for ~~windows~~ and linux as well.
**[Download](https://github.com/sysgrammar/byolisp_a/releases)**

> windows binaries only upto version early-0.6, You will have to compile the binary yourself
> linux binaries are compiled on x64 architecture

## Changelog (Begun since version early-0.4)

**Version `early-0.8**
- Implemented User Defined Functions

**Version `early-0.7`**
- Implemented Variables
- Updated Macros
- Updated input method

**Version `early-0.6`**
- Added Quoted Expressions
- Added new functions for manipulation of Q-Expressions
- Added macros for type and count checking for Q-Expression
- Evaluation support for q-expressions

**Version `early-0.5`**
- Added support for decimal input
- Added decimal calculations
- Added Symbolic Expressions like in a Typical Lisp
- Refactored Code

**Version `early-0.4`**
- Added error handling to the LISP
- Added two new opeartors: `min` and `max`
- Improved code readability
- Some cleanup and minor changes

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

Then open `main.c` and add `<editline/history.h>` to  the `#else` macro in beginning after where `<editline/readline.h>` is already present

**Termux (Mobile testing using CLang)**
Remove ```#include <editline/history.h>``` from `main.c`.

Then compile using:
```clang -std=c99 -Wall main.c mpc.c -ledit -lm -o byolisp_a```

# Syntax

## Mathematical Operations:

**Addition**

To `add` two numbers
```>>> + 5 6```

To add more than two numbers
```>>> + 5 6 7 32 543```

**Subtraction**

To `sub` two numbers
```>>> - 5 6```

To sub more than two numbers
```>>> - 543 32 7 5```

**Multiplication**

To `mul` two numbers
```>>> * 5 6```

To mul more than two numbers
```>>> * 5 6 7 32 543```

**Division(Quotient)**

To `div` two numbers
```>>> / 30 5```

**Remainder**

To find the `rem` of two numbers
```>>> % 30 5```

**Power**

To find the `pow` of two numbers(Input base first and then the power to the base should be raised):
```>>> ^ 5 6```

**Minimum**

To return the `min` of two or more numbers entered:
```>>> min 4 39```

**Maximum**

To return the `max` of two or more numbers entered:
```>>> max 4 39```

## Functions


### Quoted Expressions:

**List**

Returns a quoted expression/list of normal numbers:
```>>> list 1 2 3 4```

**Head**

Returns first element of the q-expression/list:
```>>> head {1 2 3 4}```

**Tail**

Returns the q-expression with the first element removed from it:
```>>> tail {1 2 3 4}```

**Join**

Takes two q-expressions/lists as input and joins them:
```>>> join {1 2 3} {4 5 6}```

**Eval**

Takes a Q-Expression and evaluates it as if it were a S-Expression:
```>>> eval (tail {tail tail {5 6 7}})```

**Cons**

Takes a single value and a q-expression and appends the value to the front of q-expression:
```>>> cons 1 {2 3}```

**len**
Returns the lenth of the list/q-expression:
```>>> len {2 34 5 68}```

**init**
Returns the q-expression with last element removed from it:
```>>> init {3 3 5 6 7}```

## Variables

**def**

You can declare variables using this function, this function can also alias pre existing functions present in lisp.
Although only until the value of variable defined by the user is replaced with another value. The variables in this lisp are immutable.

Takes a quoted expression list as variable name. And a S-expression as value

**Normal usage:**

``` >>> (def {num1} 10)

>>> (def {num1 num2} 69 420)

>>> (add num1 num2)
```
**Function aliasing:**

```>>> (def {jodo} add)

>>> (jodo 45 60)

>>> (def {ghatao} sub)

>>> (ghatao 60 59)
```

> Note: add, sub are builtin keywords/functions in this lisp.

## User Defined Functions

You can define your own functions using the **\\** (lambda) keyword.

It takes two arguments. A variable list and function body in form of Q-Expressions `{}`

Here are some examples.

Defining a function that first multiplies both arguments and then adds the result to first argument
```
>>> (\ {num1 num2} {+ num1 (* num1 num2)})
```

But here is the catch. This function does not have a name. We can easily use the def keyword to input a name of function and then call the function with that name.

```
>>> (def {add-mul} (\ {num1 num2} {+ num1 (* num1 num2)})) 
```

and now we can call it using the following syntax by supplying both function arguments.

```
>>> (add-mul 420 69)
```

There are more examples to these functions. But since I am lazy af I don't plan to add it and Just gonna push this readme and the changes to Github.

> There won't be any releases. You can compile the lisp on your own machine to try out.

If any of you guys would like to write proper docs and can help in testing this lisp with interesting things. Feel free to contact me.

Contact info can be found on [Sysgrammer's Contact Page](https://sysgrammer.github.io/about/)