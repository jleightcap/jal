#ifndef EMIT_H
#define EMIT_H

#include <stdio.h>

#include "parse.h"

void
emit_init(FILE* f);

void
emit_free(void);

void
emit(const struct funenv* fenv, const struct varenv* venv);

void
emit_include(const char* path);

#endif
