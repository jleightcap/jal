#ifndef UTIL_H
#define UTIL_H

#include "token.h"
#include "parse.h"

#define panic(str) \
    fprintf(stderr, str); \
    fprintf(stderr, "\n"); \
    exit(-1);

#define checktok(tok, expect, str) \
    assert(tok.type == expect && str);

int
iswhitespace(char c);

int
issymbol(const char c);

int
isnum(const char c);

unsigned long
hashstr(const char* str);

void
print_type(const enum type t);

void
print_builtin(const enum builtin b);

void
print_fenv(const struct funenv* fenv);

void
print_venv(const struct varenv* venv);

void
print_defun(const struct func* f);

void
print_func(const struct func* f, const unsigned int nest);

void
print_lit(const struct lit* l, const unsigned int nest);

void
print_var(const struct var* v, const unsigned int nest);

void
print_expr(const struct expr* e, const  unsigned int nest);

enum type
typetok_to_type(const enum toktype t);

#endif
