CC= gcc
CFLAGS= -std=c99 -Wall
LINKS= -ledit -lm

all: main.c mpc.c mpc.h
	$(CC) $(CFLAGS) main.c mpc.c $(LINKS) -o lispy

install: main.c mpc.c mpc.h
	$(CC) $(CFLAGS) main.c mpc.c $(LINKS) -o lispy
	chmod +x lispy
	cp {lispy,stdlib.bsf} /usr/bin

clean:
	rm *.c,*.h,*.md,LICENSE
	rm *.o 