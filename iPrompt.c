    /*  Contains interactive prompt code, actual project hasn't begun yet,
    but I am thinking of doing a complete rewrite with all the bonus questions so I guess I'll be uploading
    different files for this project and then declare a readme for the project                              */

#include <stdio.h>
#include <stdlib.h>

/* On windows compilation */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Readline function for windows */ 
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    return cpy;
}

/* add_history for windows */
void add_history(char* unused) {}

/* Non windows code to be included */
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main (int argc, char** argv) {

    puts("Byolisp_a version early-0.1");
    puts("Press ctrl+c to exit\n");

    /* A never ending loop, base of out interactive prompt where all commands run until explicitly close */
    while(1) {
        
        char* input = readline("byolisp> ");
        add_history(input);

        printf("Echo: %s\n", input);
        free(input);
    }

    return 0;
}