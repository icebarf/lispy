#include "mpc.h"

/* For windows only */
#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}
void add_history(char* unused) {}
#else
/* For Linux */
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv) {
    
    /* Creating parser for our language */
    mpc_parser_t* Number        = mpc_new("number");
    mpc_parser_t* Operator      = mpc_new("operator");
    mpc_parser_t* Expr          = mpc_new("expr");
    mpc_parser_t* Byolisp       = mpc_new("byolisp");

    /* Defining the parser with the following */
    mpca_lang(MPCA_LANG_DEFAULT, 
    "                                                                                                                              \
    number          : /-?[0-9]+\.?[0-9]?/;                                                                                         \
    operator        : '+' | '-' | '*' | '/' | '%' | '^' | \"add\" | \"sub\" | \"mul\" | \"div\" | \"rem\" | \"pow\";               \
    expr            : <number> | '(' <operator> <expr>+ ')';                                                                       \
    byolisp         : /^/ <operator> <expr>+ /$/;                                                                                  \
    ",
    Number, Operator, Expr, Byolisp);

    /* version info */
    puts("Byolisp version early-0.2");
    puts("Press CTRL + C to Exit");
    puts("Learning from https://buildyourownlisp.com\n");

    while(1) {
        char* input = readline("byolisp_a> ");
        add_history(input);

        /* Trying to parse the input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Byolisp, &r)) {
            /* If successful print and delete the ast */
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            /* If not successful print and delete the error */
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }  

    /* undefining the parsers */
    mpc_cleanup(4, Number, Operator, Expr, Byolisp);

    return 0;
}