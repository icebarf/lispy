#include "mpc.h"
#include <strings.h>
#include <stdlib.h>
#include <math.h>

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

/* Creating enumerations of Possible LVAL Types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

/* Creating a Lval Struct */
typedef struct lval {
    int type;
    double num;

    /* Error and symbol types will have strings as data */
    char* err;
    char* sym;
    
    /* Count and pointer to a list of lval */
    int count;
    struct lval** cell;

} lval;

/* Creating a pointer to new number of type lval */
lval* lval_num(double x) {
    lval* v =  malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

/* Creating a pointer to new error of type lval */
lval* lval_err(char* m) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m)+1);
    strcpy(v->err, m);
    return v;
}

/* Constructing a pointer a new symbol of lval */
lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

/* A pointer to a empty sexpr lval */
lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

/* A pointer/constructor to a empty qexpr lval */
lval* lval_qexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

void lval_del(lval* v) {

    switch(v->type) {
        /* Do nothing special for number type */
        case LVAL_NUM: break;

        /* For err and sym data, free their memory here */
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;

        /* If qexpr/sexpr then delete(free) all elements inside it */
        case LVAL_QEXPR:    
        case LVAL_SEXPR:
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            /* freeing memory used to contain pointers as well */
            free(v->cell);
        break;
        
    }

    /* Free memory allocated to lval struct itself*/
    free(v);
}

lval* lval_add(lval* v, lval* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

lval* lval_pop(lval* v, int i) {
    /* Find item at index "i" */
    lval* x = v->cell[i];

    /* Shifting memory after the item at "i" over the top */
    memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count-i-1));

    /* Decrease the count of items in list */
    v->count--;

    /* Reallocating the memory used */
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x;
}

lval* lval_join(lval* x, lval* y) {

    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }

    lval_del(y);
    return x;
}

lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        
        /* Print value contained within */
        lval_print(v->cell[i]);

        /* Don't print trailing space if last element */
        if( i != (v->count-1)) {
            putchar(' ');
        }
    }

    putchar(close);
}

void lval_print(lval* v) {
    switch(v->type) {
        case LVAL_NUM: printf("%lf", v->num); break;
        case LVAL_ERR: printf("Lisp error: %s", v->err); break;
        case LVAL_SYM: printf("%s", v->sym); break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
        case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
    }
}

/* Print an "lval" followed by a new line */
void lval_println(lval* v) { lval_print(v); putchar('\n');}


/* MACRO CODE FOR ERROR CHECKING IN BUILTIN FUNCS */
#define LASSERT(args, cond, err) \
    if (!(cond)) { lval_del(args); return lval_err(err); }
/* MACRO END*/

lval* lval_eval(lval* v);

/* Qexpr Functions here */
lval* builtin_list(lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_head(lval* a) {
    /* Calling Error checking macro */
    LASSERT(a, a->count == 1,
     "Function 'head' passed too many arguments. ");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
     "Function 'head' was passed wrong type of argument i.e Not Quoted Expression");
    LASSERT(a, a->cell[0]->count != 0,
     "Function 'head' was passed empty argument {}");

    /* If error checking successful, then do this: */
    /* Take the first argument */
    lval* v = lval_take(a, 0);

    /* Delete all elements that aren't head and return */
    while (v->count > 1) { lval_del(lval_pop(v, 1)); }
    return v;
}

lval* builtin_tail(lval* a ) {
    /* Calling Error checking macro */
    LASSERT(a, a->count == 1, "Function 'tail' passed too many arguments. ");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'tail' was passed wrong type of argument i.e Not Quoted Expression");
    LASSERT(a, a->cell[0]->count != 0, "Function 'tail' was passed empty argument {}");

    /* If error checking successful */
    /* Taking first argument */
    lval* v = lval_take(a, 0);

    /* Delete first element and return */
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_eval(lval* a) {
    LASSERT(a, a->count == 1, "Function 'eval' was passed too many arguments");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'eval' was passed incorrect type i.e not Quoted Expression. ");

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(x);
}

lval* builtin_join(lval* a) {

    for(int i = 0; i < a->count; i++) {
        LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
         "Function 'join' was passed incorrect type. ");
    }

    lval* x = lval_pop(a, 0);

    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval* builtin_op(lval* a, char* op) {

    /* Ensuring all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      lval_del(a);
      return lval_err("Cannot operate on non-number!");
    }
  }

    /* Pop the first element */
    lval* x = lval_pop(a, 0);

    /* If no args and sub opeartor, perform unrary negation */
    if ((strcmp(op, "-") == 0) && a->count == 0){
        x->num = -x->num;
    }
    if ((strcmp(op, "sub") == 0) && a->count == 0){
        x->num = -x->num;
    }

    /* Now while there are still elements reamining */
    while(a->count > 0) {

        /* Pop the next element */
        lval* y = lval_pop(a, 0);

        /* Peform operation/calculation */
        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "add") == 0) { x->num += y->num; }

        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "sub") == 0) { x->num -= y->num; }

        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "mul") == 0) { x->num *= y->num; }

        if (strcmp(op, "/") == 0) { if(y->num == 0){
                                        lval_del(x); lval_del(y);
                                        x = lval_err("Cannot divide by zero"); 
                                        break;
                                        }
                                     x->num /= y->num; }
        if (strcmp(op, "div") == 0) { if(y->num == 0){
                                        lval_del(x); lval_del(y);
                                        x = lval_err("Cannot divide by zero"); 
                                        break;
                                        }
                                     x->num /= y->num; }

        if (strcmp(op, "%") == 0) { if(y->num == 0){
                                        lval_del(x); lval_del(y);
                                        x = lval_err("Cannot divide by zero"); 
                                        break;
                                        }
                                     x->num = fmod(x->num, y->num); }
        if (strcmp(op, "rem") == 0) { if(y->num == 0){
                                        lval_del(x); lval_del(y);
                                        x = lval_err("Cannot divide by zero"); 
                                        break;
                                        }
                                     x->num = fmod(x->num, y->num); }

        if (strcmp(op, "^") == 0) { x->num = pow(x->num, y->num); }
        if (strcmp(op, "pow") == 0) { x->num = pow(x->num, y->num); }

        if (strcmp(op, "min") == 0) { x->num = fmin(x->num, y->num); }
    
        if (strcmp(op, "max") == 0) { x->num = fmin(x->num, y->num); }

        /* Deleting element as finished with it now */
        lval_del(y);
    }

    /* Delete input expression and return result */
    lval_del(a);
    return x;
}

lval* builtin(lval* a, char* func) {
    /* Check for qexpr builtin functions */
    if(strcmp("list", func) == 0) { return builtin_list(a); }
    if(strcmp("head", func) == 0) { return builtin_head(a); }
    if(strcmp("tail", func) == 0) { return builtin_tail(a); }
    if(strcmp("join", func) == 0) { return builtin_join(a); }
    if(strcmp("eval", func) == 0) { return builtin_eval(a); }
    if(strstr("+-/*", func)) { return builtin_op(a, func); }
    lval_del(a);
    return lval_err("Unknown Function!");
}

lval* lval_eval_sexpr(lval* v) {

    /* Evaluate children */
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    /* Error checking */
    for (int i = 0; i < v->count; i++) {
        if(v->cell[i]->type == LVAL_ERR) {
            return lval_take(v, i);
        }
    }

    /* For Empty expression */
    if (v->count == 0) { return v; }

    /* For a single expression */
    if (v->count == 1) {return lval_take(v, 0);}

    /* Ensuring first element is a symbol */
    lval* f = lval_pop(v, 0);
    if(f->type != LVAL_SYM) {
        lval_del(f); lval_del(v);
        return lval_err("S-Expression does not start with a symbol");
    }

    /* Call builtin with operator */
    lval* result = builtin(v, f->sym);
    lval_del(f);
    return result;
}

lval* lval_eval(lval* v) {
    /* Evaluate Sexpressions */
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }

    /* All other types remain the same */
    return v;
}

lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    double x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("Invalid/very large number");
}

lval* lval_read(mpc_ast_t* t) {

    /* If symbol or number return conversion to that type */
    if(strstr(t->tag, "number")) { return lval_read_num(t); }
    if(strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

    /* if root (>) or sexpr then create an empty list */
    lval* x = NULL;
    if (strcmp(t->tag, ">") == 0)     { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr")) { x = lval_sexpr(); }
    if (strstr(t->tag, "qexpr")) { x = lval_qexpr(); }

    /* Fill the entire list any valid expression contained within */
    for (int i = 0; i < t->children_num; i++) {
        if(strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if(strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if(strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if(strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if(strcmp(t->children[i]->tag, "regex") == 0)  { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

int main(int argc, char** argv) {
    
    /* Creating parser for our language */
    mpc_parser_t* Number        = mpc_new("number");
    mpc_parser_t* Symbol        = mpc_new("symbol");
    mpc_parser_t* Sexpr         = mpc_new("sexpr");
    mpc_parser_t* Qexpr         = mpc_new("qexpr");
    mpc_parser_t* Expr          = mpc_new("expr");
    mpc_parser_t* Byolisp       = mpc_new("byolisp");

    /* Defining the parser with the following */
    mpca_lang(MPCA_LANG_DEFAULT, 
    "                                                                                                                                                   \
    number          : /-?[0-9]+\\.?[0-9]?/ ;                                                                                                             \
    symbol          : '+' | '-' | '*' | '/' | '%' | '^' |                                                                                               \
                      \"add\" | \"sub\" | \"mul\" | \"div\" | \"rem\" | \"pow\" | \"min\" | \"max\"                                                     \
                    | \"list\" | \"head\" | \"tail\" | \"join\" | \"eval\" ;                                                                                                           \
    sexpr           : '(' <expr>* ')' ;                                                                                                                 \
    qexpr           : '{' <expr>* '}' ;                                                                                                                 \
    expr            : <number> | <symbol> | <sexpr> | <qexpr> ;                                                                                          \
    byolisp         : /^/ <expr>* /$/ ;                                                                                                                  \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, Byolisp);

    /* version info */
    puts("Byolisp version early-0.6");
    puts("Press CTRL + C to Exit");
    puts("Learning from http://buildyourownlisp.com\n");

    while(1) {

        char* input = readline("byolisp_a> ");
        add_history(input);

        /* Trying to parse the input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Byolisp, &r)) {
            lval* x = lval_eval(lval_read(r.output));
            lval_println(x);
            lval_del(x);
            mpc_ast_delete(r.output);
      
        } else {    
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
    
        free(input);
    
    }  

    /* undefining the parsers */
    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Byolisp);

    return 0;
}