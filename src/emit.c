#include <stdio.h>

#include "parse.h"
#include "emit.h"

// architectures
#include "riscv64.h"

void
emit(FILE* f, const struct funenv* fenv, const struct varenv* venv)
{
    emit_riscv64(f, fenv, venv);
}
