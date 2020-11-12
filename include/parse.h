#ifndef PARSE_H
#define PARSE_H

#include "token.h"

// maximum number of arguments to a function
#ifndef MAXARGS
#define MAXARGS 10
#endif
// maximum number of expressions in a function definition
#ifndef MAXEXPRS
#define MAXEXPRS 50
#endif

// ========================================================================= //
//                                                                           //
// PARSE DEFINITIONS                                                         //
//                                                                           //
// ========================================================================= //

// TYPES - assigned to symbols
enum type {
    INT,
    STRING,
    VOID
};

// LITERALS ================================================================= //
struct lit {
    enum type t;
    union {
        int integer;
        char string[MAX_STRLEN];
    } litval;
};


// VARIABLES ================================================================ //
struct var {
    enum type t;
    unsigned long hash;
    struct lit lit;
};
struct varenv {
    struct var env[ENV_SIZE];
};

// FUNCTIONS ================================================================ //
enum builtin {
    // arithmetic
    F_ADD,
    F_SUB,
    F_MUL,
    F_DIV,
    F_MOD,
    
    // binary operators
    F_NOT,
    F_AND,
    F_OR,
    F_LSL,
    F_LSR,

    // comparators
    F_EQ,
    F_NE,
    F_GT,
    F_LT,
    F_GE,
    F_LE,
   
    // special function tokens
    F_ASSGN,
    F_INC,
    F_PEQ,
    F_DEC,
    F_MEQ,
    F_QUI,

    // hashed symbols
    F_MAIN,
    F_DEFUN,
    F_DEVAR,
    F_RET,
    F_WHILE
};
enum ftype {
    FT_DEF,
    FT_BUILTIN,
    FT_IMPORT,
    FT_CALL
};
struct func {
    // metadata
    enum type t;
    enum ftype ft;
    union {
        unsigned long hash;
        enum builtin b;
    } name; // tagged by ft
    
    // args
    unsigned int argnum;
    struct {
        // arguments are variables, use hash to look up in venv
        unsigned long arghash[MAXARGS];
        // arguments are just a type, for builtin functions
        enum type argt[MAXARGS];
    } args; // taged by ft
    
    // body
    unsigned int exprs;
    struct expr* body[MAXEXPRS];

    // variable environment
    struct varenv* venv;
};
struct funenv {
    struct func env[ENV_SIZE];
};

// EXPERSSIONS ============================================================= //
enum exprtypes {
    FUNCTION,
    LITERAL,
    VARIABLE
};
struct expr {
    enum exprtypes exprtype;
    union {
        // FUNCTIONS
        struct func func;
        // LITERALS
        struct lit lit;
        // VARIABLES
        struct var var;
    } e;
};


// ======================================================================== //
//                                                                          //
// PARSE FUNCTIONS                                                          //
//                                                                          //
// ======================================================================== //

// initialize/free an expression
struct expr*
expr_init(void);

void
func_init(struct func* f, const enum type t, const enum ftype ft,
          const unsigned long hash, const enum builtin b,
          const unsigned int argnum, const unsigned int exprs,
          const struct varenv* parent_venv);

void
expr_free(struct expr* e);

// free environments (collection of expressions)
void
funenv_free(struct funenv* fenv);

void
varenv_free(struct varenv* venv);

enum type
typetok_to_type(const enum toktype t);

// parse tokens in a given function environment (fenv) and variable environment (venv)
void
parse(struct funenv* fenv, struct varenv* venv);

// parse function signatures
void
parse_expr(struct expr* e,
           struct funenv* fenv, struct varenv* venv);

void
parse_defun(struct funenv* fenv, struct varenv* venv);

void
parse_devar(struct funenv* fenv, struct varenv* venv);

void
parse_import(struct funenv* fenv, struct varenv* venv);

void
parse_ret(struct expr* e,
          struct funenv* fenv, struct varenv* venv);

void
parse_print(struct expr* e,
            struct funenv* fenv, struct varenv* venv);

void
parse_quinary(struct expr* e,
              struct funenv* fenv, struct varenv* venv);

void
parse_while(struct expr* e,
            struct funenv* fenv, struct  varenv* venv);

void
parse_assign(struct expr* e,
             struct funenv* fenv, struct varenv* venv);

void
parse_uniop(struct expr* e,
            struct funenv* fenv, struct varenv* venv);

void
parse_binop(struct expr* e,
            struct funenv* fenv, struct varenv* venv);

void
parse_call(struct expr* e,
           struct funenv* fenv, struct varenv* venv);

#endif
