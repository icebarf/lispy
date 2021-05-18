CC= gcc
CFLAGS= -std=c99 -Wall
LINKS= -ledit -lm
LINKS_WIN= -lm
STDLIB_PATH=/usr/bin

all: install

### Instructions for windows users																	  ###
#																										#
# In the following command:																				#
#			$(CC) $(CFLAGS) main.c mpc.c $(LINKS) -o lispy												#
#																										#
# Replace $(LINKS) with $(LINKS_WIN) and remove remaining two commands starting with chmod and mv		#
#																										#
###																									  ###

install: main.c mpc.c mpc.h
	clear && $(CC) $(CFLAGS) main.c mpc.c $(LINKS) -o lispy
	chmod +x lispy
	mv {lispy,stdlib.bsf} $(STDLIB_PATH)

clean:
	rm *.o