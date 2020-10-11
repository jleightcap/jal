#ifndef PARSE_H
#define PARSE_H

#include "token.h"

// maximum number of arguments to a function
#define MAXARGS 10
// maximum number of expressions in a function definition
#define MAXEXPRS 50

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

// FUNCTIONS ================================================================ //
enum builtin {
    F_ADD,
    F_SUB,
    F_MUL,
    F_DIV,
    F_MOD
};
enum ftype {
    BUILTIN,
    TABLE
};
struct func {
    enum type t;
    enum ftype ft;
    union {
        unsigned long hash;
        enum builtin b;
    } name;
    unsigned int exprs;
    struct expr* body[MAXEXPRS];
};
struct funenv {
    struct func env[ENV_SIZE];
};

// VARIABLES ================================================================ //
struct var {
    enum type t;
    struct expr* body;
};
struct varenv {
    struct var env[ENV_SIZE];
};

// LITERALS ================================================================= //
struct lit {
    enum type t;
    union {
        int integer;
        char string[MAX_STRLEN];
    } litval;
};

// EXPERSSIONS ============================================================= //
enum exprtypes {
    FUNCTION,
    LITERAL
};
struct expr {
    enum exprtypes exprtype;
    union {
        // FUNCTIONS
        struct func func;
        // LITERALS
        struct lit lit;
    } e;
};


// ======================================================================== //
//                                                                          //
// PARSE FUNCTIONS                                                          //
//                                                                          //
// ======================================================================== //

// parse tokens in a given function environment (fenv) and variable environment (venv)
void
parse(struct funenv* fenv, struct varenv* venv);

// initialize/free an expression
struct expr*
expr_init(void);

void
expr_free(struct expr* e);

// free environments (collection of expressions)
void
funenv_free(struct funenv* fenv);

void
varenv_free(struct varenv* venv);

enum type
typetok_to_type(const enum toktype t);

#endif
