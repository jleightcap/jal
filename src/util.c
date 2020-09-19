#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"

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

char*
substr(const char* src, const int offset, const int len)
{
    char* buf = malloc((len + 1) * sizeof(char));
    memcpy(buf, &src[offset], len);
    buf[len + 1] = '\0';
    return buf;
}
