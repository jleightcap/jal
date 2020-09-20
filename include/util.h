#ifndef UTIL_H
#define UTIL_H

/* things that should be macros, but no preprocessor. */

int
iswhitespace(char c);

int
issymbol(const char c);

int
isnum(const char c);

unsigned long
hashstr(const char* str);

#endif
