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

// tokens that map direction to function
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
    LITERAL
};

// an expression
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

        // LITERALS
        struct {
            enum type t;
            union {
                int integer;
            } litval;
        } literal;
    } expression;
};

// FUNCTIONS

#define MAXARGS 10
#define MAXEXPRS 50
struct func {
    // return type
    enum type type;
    unsigned int exprs;
    struct expr* body[MAXEXPRS];
};
struct funenv {
    struct func env[ENV_SIZE];
};

// VARIABLES
struct var {
    enum type type;
    // note: a variable's expression body must be a literal type
    struct expr body;
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

enum type
typetok_to_type(const enum toktype t);

#endif
