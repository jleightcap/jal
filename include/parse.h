#ifndef PARSE_H
#define PARSE_H

#include "token.h"

// ========================================================================= //
//                                                                           //
// PARSE DEFINITIONS                                                         //
//                                                                           //
// ========================================================================= //

// types assigned to symbols
enum type {
    INT
};


enum intrinsic {
    PLUS,
    MINUS,
    TIMES,
    DIVIDE
};

enum exprtype {
    // variable expression
    UNARY,
    BINARY,

    // literal expression
    LITERAL_INT
};

struct expr {
    enum exprtype type;
    union {
        // UNARY
        struct {
            enum intrinsic op;
            struct expr* arg;
        } unary;

        // BINARY
        struct {
            enum intrinsic op;
            struct expr* arg1;
            struct expr* arg2;
        } binary;

        // LITERAL_INT
        int val;
    } body;
};

// FUNCTIONS
struct func {
    enum type type;
    struct expr* body;
};
struct funenv {
    struct func env[ENV_SIZE];
};

// VARIABLES
struct var {
    // what's different about a variable and a function?
    // the body of a variable is evalulated at compile time
    enum type type;
    struct expr* body;
};
struct varenv {
    struct var env[ENV_SIZE];
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

#endif
