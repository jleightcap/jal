#ifndef SCAN_H
#define SCAN_H

#include "slist.h"

#define ENV_SIZE 1000

enum toktype {
    INTLIT, SYM
};

struct token {
    enum toktype type;
};

struct environ {
    struct token env[ENV_SIZE];
};

char*
scan(const char* file, const int len);

#endif
