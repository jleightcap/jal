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
    TYPE_VOID,          // void

    // atoms
    A_INT,              // number
    A_STR,              // string

    // symbol
    SYM,                // symbol

    // reserved symbols
    MAIN,
    DEFUN,
    DEVAR,
    RET,
    PRINT,

    // operators
    ADD,                // (+ a b)
    SUB,                // (- a b)
    MUL,                // (* a b)
    DIV,                // (/ a b)
    MOD,                // (% a b)
    LSL,                // (<< a b)
    LSR,                // (>> a b)
    ASSIGN,             // (= a b)
    NOT,                // (! a)
    AND,                // (& a b)
    OR,                 // (| a b)

    // binary comparators
    EQ,                 // (== a b)
    NE,                 // (!= a b)
    GT,                 // (>  a b)
    LT,                 // (<  a b)
    GE,                 // (>= a b)
    LE,                 // (<= a b)

    // conditionals
    QUINARY,            // (? cond conseq else)
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
