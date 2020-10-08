#ifndef TOKEN_H
#define TOKEN_H

#define ENV_SIZE 1000

// ========================================================================= //
//                                                                           //
// TOKEN DEFINITIONS                                                         //
//                                                                           //
// ========================================================================= //

enum toktype {
    // parsing tokens
    LPAREN,             // '('
    RPAREN,             // ')'
    END,                // EOF

    // type declarations
    TYPE_INT,           // "int" integer type
    TYPE_STR,           // "string" type

    // atoms
    SYM,                // symbol
    NUM,                // number
    STR,                // string

    // reserved symbols
    DEFUN,              // (defun [signature] [body])
    DEVAR,              // (defar [var] [body])
    MAIN,               // (main int) [args]
    RETRN,              // (return [expr])
    TRUE,               // "true"
    FALSE,              // "false"
    PRINT,              // "print"


    // binary operators
    ADD,                // (+ a b)
    SUB,                // (- a b)
    MUL,                // (* a b)
    DIV,                // (/ a b)
    MOD,                // (% a b)

    // binary comparators
    EQ,                 // (== a b)
    NE,                 // (!= a b)
    GT,                 // (>  a b)
    LT,                 // (<  a b)
    GE,                 // (>= a b)
    LE                  // (<= a b)
};

#define MAX_STRLEN 100
struct token {
    enum toktype type;
    union {
        // literals
        int num;
        char str[MAX_STRLEN + 1]; // add slot at end for null terminator
        // symbol hash
        long hash;
    } value;
};



// ========================================================================= //
//                                                                           //
// TOKEN FUNCTIONS                                                           //
//                                                                           //
// ========================================================================= //

void
setstream(const char stream[], int streamlen);

struct token
scan();

#endif
