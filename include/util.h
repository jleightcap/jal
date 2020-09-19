#ifndef UTIL_H
#define UTIL_H

/* things that should be macros, but no preprocessor. */

int
iswhitespace(char c);

int
issymbol(const char c);

char*
substr(const char* src, const int offset, const int len);

#endif
