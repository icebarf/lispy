
# Project Info

Lispy is a lisp implementation which was invented by Daniel holden [in his book BuildYourOwnLisp](http://buildyourownlisp.com/). It is a dynamically typed interpreted language.

More info about this can be found on my [website](https://sysgrammer.github.io/docs/lispy_docs.html).

## Installation and compilation

### Depenencies
- Make
- GCC
- libedit

### Linux

#### Debian/Ubuntu
If you are on Debian/Ubuntu or any other distribution, you need to make a change in the `main.c` file. If you are using any other distro feel free to skip this part

- Make sure libedit is installed on your machine
- Open main.c in your favorite editor.
- Go to line 20. Press enter and type the follow and then save and exit

To type:
```c
#include <editline/history.h>
```

#### Any other distro or Ubuntu/Debian
- Install make on your machine
- `git clone https://github.com/sysgrammer/lispy.git`
- `cd lispy`
- `sudo make install` to install to `/usr/bin` or to run without `./`
- `make` to simply compile

Optional:
- `make clean`

## Documentation and Syntax

Documentation and a guide to lispy can be found [here](https://sysgrammer.github.io/docs/lispy_docs.html)


## Other cool stuff made by friends

Check out [Wise Lang](https://github.com/Sakon13/wiselang) made by [Sakon](https://github.com/Sakon13) and [Blackmamba](https://github.com/BLA4KM4MBA).
It is an esoteric programming language made for fun.


Contact info can be found on [Sysgrammer's Website](https://sysgrammer.github.io/)
