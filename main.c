#define _CRT_SECURE_NO_WARNINGS
#include "mpc.h"
#include <string.h>
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
#endif

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

/* Creating enumerations of Possible LVAL Types */
enum {  LVAL_NUM, LVAL_ERR, LVAL_SYM,
        LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN
};

/*  To get a lval* we deference lbuiltin and then call it with lenv* and lval* which will return a lval* 
  * This is a function pointer *
*/
typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
    int type;
    double num;

    char* err;
    char* sym;
    lbuiltin fun;

    int count;
    struct lval** cell;

};


/* Creating a pointer to new number of type lval */
lval* lval_num(double x) {
    lval* v =  malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

/* Creating a pointer to new error of type lval */
lval* lval_err(char* fmt, ...) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;

    /* Create a va list and initialising */
    va_list va;
    va_start(va, fmt);

    /* Allocating 1024 Bytes of space */
    v->err = malloc(1024);

    /* print the error first with max chars */
    vsnprintf(v->err, 1023, fmt, va);

    /* Reallocating the number of bytes actually used */
    v->err = realloc(v->err, strlen(v->err)+1);

    /* Cleanup our list */
    va_end(va);

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

/* lval function constructor */
lval* lval_fun(lbuiltin func) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->fun = func;
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
        /* Do nothing special for number and function type */
        case LVAL_FUN: break;
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

lval* lval_copy(lval* v) {

  lval* x = malloc(sizeof(lval));
  x->type = v->type;
  
  switch (v->type) {
    
    /* Copy Functions and Numbers Directly */
    case LVAL_FUN: x->fun = v->fun; break;
    case LVAL_NUM: x->num = v->num; break;
    
    /* Copy Strings using malloc and strcpy */
    case LVAL_ERR:
      x->err = malloc(strlen(v->err) + 1);
      strcpy(x->err, v->err); break;
      
    case LVAL_SYM:
      x->sym = malloc(strlen(v->sym) + 1);
      strcpy(x->sym, v->sym); break;
    
    /* Copy Lists by copying each sub-expression */
    case LVAL_SEXPR:
    case LVAL_QEXPR:
      x->count = v->count;
      x->cell = malloc(sizeof(lval*) * x->count);
      for (int i = 0; i < x->count; i++) {
        x->cell[i] = lval_copy(v->cell[i]);
      }
    break;
  }
  
  return x;
}

lval* lval_add(lval* v, lval* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

lval* lval_join(lval* x, lval* y) {

    for (int i = 0; i < y->count; i++) {
        x = lval_add(x, y->cell[i]);
    }

    free(y->cell);
    free(y);
    return x;
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

lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

void lval_print(lval* v);

void lval_print_expr(lval* v, char open, char close) {
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
        case LVAL_FUN: printf("<function>");                        break;
        case LVAL_NUM: printf("%.5lf", v->num);                     break;
        case LVAL_ERR: printf("Lisp error: %s", v->err);            break;
        case LVAL_SYM: printf("%s", v->sym);                        break;
        case LVAL_SEXPR: lval_print_expr(v, '(', ')');              break;
        case LVAL_QEXPR: lval_print_expr(v, '{', '}');              break;
    }
}

/* Print an "lval" followed by a new line */
void lval_println(lval* v) { lval_print(v); putchar('\n');}


char* ltype_name(int t) {
    switch(t) {
        case LVAL_FUN: return "Function";
        case LVAL_NUM: return "Number";
        case LVAL_SYM: return "Symbol";
        case LVAL_ERR: return "Error";
        case LVAL_QEXPR: return "Q-Expression";
        case LVAL_SEXPR: return "S-Expression";
        default: return "Unknown";
    }
}

/* The Lisp environment */
struct lenv {
    int count;
    char** syms;
    lval** vals;
};

/* Construct lisp environment  */
lenv* lenv_new(void) {
    lenv* e = malloc(sizeof(lenv));
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

/* Deconstruct lisp environment */
void lenv_del(lenv* e){
    /* Iterating over all items and deleting them */
    for(int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        lval_del(e->vals[i]);
    }

    /* Freeing allocated memory for lists */
    free(e->syms);
    free(e->vals);
    free(e);
}

lval* lenv_get(lenv* e, lval* k){
    /* Go through all items in environment */
    for (int i = 0; i < e->count; i++){
        /* Check if the stored string matches input string *
         * if it does then return a copy of its value */
         if(strcmp(e->syms[i], k->sym) == 0) {
             return lval_copy(e->vals[i]);
         }
    }

    /* If no symbol found return error */
    return lval_err("Unbound Symbol %s", k->sym);
}

void lenv_put(lenv* e, lval* k, lval* v) {
    /* Iterate over all items in environment to see if variable exists already */
    for(int i = 0; i < e->count; i++) {

        /* If variable is found then delete item at position *
         * and replace its value supplied by user */
         if(strcmp(e->syms[i], k->sym) == 0){
             lval_del(e->vals[i]);
             e->vals[i] = lval_copy(v);
             return;
         }
    }

    /* If no existing varibale is found then allocate space foe new values to be entered */
    e->count++;
    e->vals = realloc(e->vals, sizeof(lval*) * e->count);
    e->syms = realloc(e->syms, sizeof(char*) * e->count);

    /* Copy contents of lval and symbol to newly allocated space */
    e->vals[e->count-1] = lval_copy(v);
    e->syms[e->count-1] = malloc(strlen(k->sym)+1);
    strcpy(e->syms[e->count-1], k->sym);
}

/* MACRO CODE FOR ERROR CHECKING IN BUILTIN FUNCS */

#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); lval_del(args); return err; }

#define LASSERT_ELIST(func, args, index) \
    LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index)

#define LASSERT_TYPE(func, args, index, expect) \
    LASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num) \
    LASSERT(args, args->count == num, \
    "Funcion '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
    func, args->count, num)
/* MACRO END*/

lval* lval_eval(lenv* e, lval* v);

/* Qexpr Functions here */
lval* builtin_list(lenv* e, lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_head(lenv* e, lval* a) {
    /* Calling Error checking macro */
    LASSERT_NUM("head", a, 1);
    LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
    LASSERT_ELIST("head", a, 0);

    /* If error checking successful, then do this: */
    /* Take the first argument */
    lval* v = lval_take(a, 0);

    /* Delete all elements that aren't head and return */
    while (v->count > 1) { lval_del(lval_pop(v, 1)); }
    return v;
}

lval* builtin_tail(lenv* e, lval* a ) {
    /* Calling Error checking macro */
    LASSERT_NUM("tail", a, 1);
    LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
    LASSERT_ELIST("tail", a, 0);

    /* If error checking successful */
    /* Taking first argument */
    lval* v = lval_take(a, 0);

    /* Delete first element and return */
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_eval(lenv* e, lval* a) {
    LASSERT_NUM("eval", a, 1);
    LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {

    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE("join", a, i, LVAL_QEXPR)
    }

    lval* x = lval_pop(a, 0);

    while (a->count) {
        lval* y = lval_pop(a, 0);
        x = lval_join(x, y);
    }

    lval_del(a);
    return x;
}

lval* builtin_cons(lenv* e, lval* a) {
  /* Check input for error */
    LASSERT_NUM("cons", a, 2);
    LASSERT_TYPE("cons", a, 0, LVAL_NUM);
    LASSERT_TYPE("cons", a, 1, LVAL_QEXPR);

    /*Creating new lvals and assigning vals */
    lval* val = lval_qexpr();
    lval* y = lval_pop(a,0);
    val = lval_add(val, y);
    lval* rem_val = lval_pop(a, 0);
    
    
    while(rem_val->count) {
      val = lval_add(val, lval_pop(rem_val, 0));
    }

    lval_del(a); lval_del(rem_val);
    return val;
}

lval* builtin_len(lenv* e, lval* a) {
    LASSERT_NUM("len", a, 1);
    LASSERT_TYPE("len", a, 0, LVAL_QEXPR);
    
    /* Get count/no. of elements */
    double count = a->cell[0]->count;

    /* Return and convert to lisp value */
    return lval_num(count);

}

lval* builtin_init(lenv* e, lval* a) {
    LASSERT_NUM("init", a, 1);
    LASSERT_TYPE("init", a, 0, LVAL_QEXPR);
    LASSERT_ELIST("init", a, 0);

    lval* val = lval_qexpr();
    lval* val2 = lval_pop(a, 0);
    
    while(val2->count > 1) {
        lval* y = lval_pop(val2, 0);
        val = lval_add(val, y);
    }

    lval_del(a); lval_del(val2);
    return val;
}

lval* builtin_op(lenv* e, lval* a, char* op) {

    /* Ensuring all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(op, a, i, LVAL_NUM);
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

        if (strcmp(op, "-") == 0) { x->num -= y->num; }

        if (strcmp(op, "*") == 0) { x->num *= y->num; }

        if (strcmp(op, "/") == 0) { if(y->num == 0){
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

lval* builtin_add(lenv* e, lval* a) {
    return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
    return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
    return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
    return builtin_op(e, a, "/");
}

lval* builtin_rem(lenv* e, lval* a) {
    return builtin_op(e, a, "%");
}

lval* builtin_pow(lenv* e, lval* a) {
    return builtin_op(e, a, "pow");
}

lval* builtin_min(lenv* e, lval* a) {
    return builtin_op(e, a, "min");
}

lval* builtin_max(lenv* e, lval* a) {
    return builtin_op(e, a, "max");
}

/* NOTE NOTE NOTE NOTE NOTE *
 * The name for function bultin_var in book is bultin_def, but I am using var instead of def in every instance *
 */

lval* builtin_var(lenv* e, lval* a) {

    LASSERT_TYPE("var", a, 0, LVAL_QEXPR);

    /* First argument is symbol list */
    lval* syms = a->cell[0];

    /* Ensuring all members of list are symbols */
    for (int i = 0; i < syms->count; i++){
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
        "Function 'var' cannot define non-symbols. "
        "Got %s. Expected %s.",
        ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
    }

    /* Check for correct number of symbols and values */
    LASSERT(a, (syms->count == a->count-1),
    "Function 'var' passed too many arguments for symbols. "
    "Got %i. Expected %i.",
    syms->count, a->count-1);

    /* Assign copies of values to symbols */
    for (int i = 0; i < syms->count; i++) {
        lenv_put(e, syms->cell[i], a->cell[i+1]);
    }

    lval_del(a);
    return lval_sexpr();
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
    lval* k = lval_sym(name);
    lval* v = lval_fun(func);
    lenv_put(e, k, v);
    lval_del(k); lval_del(v);
}

void lenv_add_builtins(lenv* e) {
    /* Variable functions */
    lenv_add_builtin(e, "var", builtin_var);

    /* List Functions */
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "len", builtin_len);
    lenv_add_builtin(e, "init", builtin_init);

    /* Mathematical Functions */
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "add", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "sub", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "mul", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "div", builtin_div);
    lenv_add_builtin(e, "%", builtin_rem);
    lenv_add_builtin(e, "rem", builtin_rem);
    lenv_add_builtin(e, "^", builtin_pow);
    lenv_add_builtin(e, "pow", builtin_pow);
    lenv_add_builtin(e, "min", builtin_min);
    lenv_add_builtin(e, "max", builtin_max);
}

lval* lval_eval_sexpr(lenv* e, lval* v) {

    /* Evaluate children */
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
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

    /* Ensuring first element is a function */
    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_FUN) {
        lval* err = lval_err(
            "S-Expression starts with incorrect type."
            "Got %s, Expected %s.",
            ltype_name(f->type), ltype_name(LVAL_FUN));
        lval_del(f); lval_del(v);
        return err;
    }

    /* If is a function, then call the functions to get result */
    lval* result = f->fun(e, v);
    lval_del(f);
    return result;
}

lval* lval_eval(lenv* e, lval* v) {
    /* For Symbols */
    if (v->type == LVAL_SYM) {
        lval* x = lenv_get(e, v);
        lval_del(v);
        return x;
    }
    /* Evaluate Sexpressions */
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }

    /* All other types remain the same */
    return v;
}

/* Reading of input occurs here */
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
    number          : /-?[0-9]+\\.?[0-9]?/ ;                                                                                                            \
    symbol          : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&%]+/ ;                                                                                                      \
    sexpr           : '(' <expr>* ')' ;                                                                                                                 \
    qexpr           : '{' <expr>* '}' ;                                                                                                                 \
    expr            : <number> | <symbol> | <sexpr> | <qexpr> ;                                                                                         \
    byolisp         : /^/ <expr>* /$/ ;                                                                                                                 \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, Byolisp);

    /* version info */
    puts("Byolisp version early-0.7");
    puts("Press CTRL + C to Exit");

    lenv* e = lenv_new();
    lenv_add_builtins(e);
    while(1) {

        char* input = readline(">>> ");
        add_history(input);

        /* Trying to parse the input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Byolisp, &r)) {
            if(strstr(input, "exit")) {
                break;
            }
            lval* x = lval_eval(e, lval_read(r.output));
            lval_println(x);
            lval_del(x);
            mpc_ast_delete(r.output);
      
        } else {    
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
    
        free(input);
    
    } 

    lenv_del(e);
    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Byolisp);

    return 0;
}