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

    // atoms
    NUM,                // number
    SYM,                // symbol

    // reserved symbols
    MAIN,               // (main int) [args]
    DEFUN,              // (defun [signature] [body])
    DEVAR,              //  (defar [var] [body])
    RETRN,              // (return [expr])
    TRUE,               // "true"
    FALSE,              // "false"

    // TYPES
    TYPE_INT,           // "int" integer type

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

struct token {
    enum toktype type;
    union {
        int num;
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
