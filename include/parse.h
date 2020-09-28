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

// functions
struct func {
    enum type type;
    struct expr* body;
};
struct funenv {
    struct func env[ENV_SIZE];
};

// variables
struct var {
    enum type type;
    union {
        int integer;
    } value;
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

#endif
