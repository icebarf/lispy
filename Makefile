CC= gcc
CFLAGS= -std=c99 -Wall
LINKS= -ledit -lm

STDLIB_PATH=/usr/bin

all: install

install: main.c mpc.c mpc.h
	$(CC) $(CFLAGS) main.c mpc.c $(LINKS) -o lispy
	chmod +x lispy
	mv {lispy,stdlib.bsf} $(STDLIB_PATH)

clean:
	sudo rm /usr/bin/lispy /usr/bin/stdlib.bsf
