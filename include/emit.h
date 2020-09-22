#ifndef EMIT_H
#define EMIT_H

#include <stdio.h>

#include "token.h"

void
emit(FILE* f, const struct funenv* fenv, const struct varenv* venv);

#endif
