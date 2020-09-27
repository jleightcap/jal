#ifndef RISCV64_H
#define RISCV64_H

#include <stdio.h>

#include "parse.h"

void
emit_riscv64(FILE* f, const struct funenv* fenv, const struct varenv* venv);

#endif
