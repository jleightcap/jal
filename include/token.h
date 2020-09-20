#ifndef SCAN_H
#define SCAN_H
#define ENV_SIZE 1000

enum toktype {
    // parse tokens
    LPAREN,             // '('
    RPAREN,             // ')'
    END,                // EOF

    // atoms
    INTLIT,             // number
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
    NE,      // (!= a b)
    GT,      // (>  a b)
    LT,      // (<  a b)
    GE,      // (>= a b)
    LE       // (<= a b)
};

struct token {
    enum toktype type;
    union {
        int num;
        long hash;
        char _; // no value
    } value;
};

struct funenv {
    struct token env[ENV_SIZE];
};

struct varenv {
    struct token env[ENV_SIZE];
};

struct token
scan(const char* file, const int len);

#endif
