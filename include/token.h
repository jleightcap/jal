#ifndef SCAN_H
#define SCAN_H

#define ENV_SIZE 1000

enum toktype {
    // parsing tokens
    LPAREN,             // '('
    RPAREN,             // ')'
    END,                // EOF

    // atoms
    NUM_LIT,            // number
    SYM,                // symbol with not determined type
    SYMVAR,             // non-reserved variable symbol
    SYMFUN,             // non-reserved function symbol

    // reserved symbols
    MAIN,               // (main int) [args]
    DEFUN,              // (defun [signature] [body])
    DEVAR,              //  (defar [var] [body])
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
    LE = 1<<30                // (<= a b)
};

enum type {
    INT
};

struct token {
    enum toktype type;
    union {
        int num;
        long hash;
    } value;
};

// environment definitions

struct expr {
    int val; // haha I wish
};

struct func {
    enum type type;
    struct expr body;
};

struct funenv {
    struct func env[ENV_SIZE];
};

struct varenv {
    struct token env[ENV_SIZE];
};

void
setstream(const char stream[], int streamlen);

struct token
scan();

#endif
