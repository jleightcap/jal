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

struct expr {
    int val; // haha I wish
};

// functions
struct func {
    enum type type;
    struct expr body;
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

void
parse(struct funenv* fenv, struct varenv* venv);

#endif
