#include "mpc.h"
#include <strings.h>
#include <stdlib.h>
#include <math.h>

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

/* Creating enumerations of Possible LVAL Error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Creating enumerations of Possible LVAL Types */
enum { LVAL_NUM, LVAL_ERR };

/* Creating a Lval Struct */
typedef struct {
    int type;
    long num;
    int err;
} lval;

/* Creating a new number of type lval */
lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

/* Creating a new error of type lval */
lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

/* Print a lval */
void lval_print(lval v) {
    switch(v.type) {
        /* If we a number we print it and break out of the switch */
        case LVAL_NUM: printf("%li", v.num); break;

        /* In case we a number we do the following  */
        case LVAL_ERR: 
            /* Check error type and print it */
            if (v.err == LERR_DIV_ZERO) {
                printf("Lisp Error: Dividing by zero");
            }
            if (v.err == LERR_BAD_OP) {
                printf("Lisp Error: Invalid Operator! Refer to documentation to see the list of opeartors");
            }
            if (v.err == LERR_BAD_NUM) {
                printf("Lisp Error: Number is too large or input is not a number");
            }
        break;
    }
}

/* Print an "lval" followed by a new line */
void lval_println(lval v) { lval_print(v); putchar('\n');}

/* Calculation performed in this function after evaluating opeartor string */
lval eval_op(lval x, char* op, lval y) {

    /* If either value is an error don't compute just return */
    if (x.type == LVAL_ERR) {return x;}
    if (y.type == LVAL_ERR) {return y;}

    /* Otherwise perform the following opeartions */

    if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
    if (strcmp(op, "add") == 0) { return lval_num(x.num + y.num); }
    
    if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
    if (strcmp(op, "sub") == 0) { return lval_num(x.num - y.num); }
    
    if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
    if (strcmp(op, "mul") == 0) { return lval_num(x.num * y.num); }

    if (strcmp(op, "/") == 0) { return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num); }
    if (strcmp(op, "div") == 0) { return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num); }

    if (strcmp(op, "%") == 0) { return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num % y.num); }
    if (strcmp(op, "rem") == 0) { return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num % y.num); }

    if (strcmp(op, "^") == 0) { return lval_num(pow(x.num, y.num)); }
    if (strcmp(op, "pow") == 0) { return lval_num(pow(x.num, y.num)); }
    
    if (strcmp(op, "min") == 0) { return lval_num(fmin(x.num, y.num)); }
    
    if (strcmp(op, "max") == 0) { return lval_num(fmax(x.num, y.num)); }

    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
    
    /* If tagged as number return it directly. */ 
    if (strstr(t->tag, "number")) {
        /* Check error for conversion */
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }
  
    /* The operator is always second child. */
    char* op = t->children[1]->contents;

    /* We store the third child in `x` */
    lval x = eval(t->children[2]);
  
    /* Iterate the remaining children and combining. */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }
  
    return x;  
}

int main(int argc, char** argv) {
    
    /* Creating parser for our language */
    mpc_parser_t* Number        = mpc_new("number");
    mpc_parser_t* Operator      = mpc_new("operator");
    mpc_parser_t* Expr          = mpc_new("expr");
    mpc_parser_t* Byolisp       = mpc_new("byolisp");

    /* Defining the parser with the following */
    mpca_lang(MPCA_LANG_DEFAULT, 
    "                                                                                                                                                   \
    number          : /-?[0-9]+/;                                                                                                                       \
    operator        : '+' | '-' | '*' | '/' | '%' | '^' | \"add\" | \"sub\" | \"mul\" | \"div\" | \"rem\" | \"pow\" | \"min\" | \"max\";                \
    expr            : <number> | '(' <operator> <expr>+ ')';                                                                                            \
    byolisp         : /^/ <operator> <expr>+ /$/;                                                                                                       \
    ",
    Number, Operator, Expr, Byolisp);

    /* version info */
    puts("Byolisp version early-0.3");
    puts("Press CTRL + C to Exit");
    puts("Learning from https://buildyourownlisp.com\n");

    while(1) {
        char* input = readline("byolisp_a> ");
        add_history(input);

        /* Trying to parse the input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Byolisp, &r)) {
      
            lval result = eval(r.output);
            lval_println(result);
            mpc_ast_delete(r.output);
      
        } else {    
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
    
        free(input);
    
    }  

    /* undefining the parsers */
    mpc_cleanup(4, Number, Operator, Expr, Byolisp);

    return 0;
}