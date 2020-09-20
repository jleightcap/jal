#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "token.h"

int
iswhitespace(const char c)
{
    return (c == '\t' || c == '\n' || c == '\v' || c == ' ' || c == EOF);
}

int
issymbol(const char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

int
isnum(const char c)
{
    return (c >= '0' && c <= '9');
}

unsigned long
hashstr(const char* str)
{
    assert(strlen(str) > 0);
    unsigned long ii;
    unsigned long hash;
    hash = str[0];
    for(ii = 0; ii < strlen(str); ii++) {
        hash = (hash * 67 + str[ii]) % ENV_SIZE;
    }
    return hash;
}
