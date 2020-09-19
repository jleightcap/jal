#ifndef DEFS_H
#define DEFS_H

struct token {
    int token;
    int intvalue;
};

enum {
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT
};

#endif
