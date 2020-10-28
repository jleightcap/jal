#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "emit.h"
#include "util.h"

// architectures
#include "riscv64.h"
#include "z80.h"

void
emit(FILE* f, const struct funenv* fenv, const struct varenv* venv)
{
#ifdef _RISCV_64
    emit_riscv64(f, fenv, venv);
#else
#ifdef _Z80
    emit_z80(f, fenv, venv);
#else
    panic("Must define a supported architecture with -DARCH!\n");
#endif
#endif
}
