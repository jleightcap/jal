#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "emit.h"
#include "util.h"

// architectures
#include "6502.h"
#include "riscv64.h"

void
emit(FILE* f, const struct funenv* fenv, const struct varenv* venv)
{
#ifdef _RISCV_64
    emit_riscv64(f, fenv, venv); goto done;
#endif
#ifdef _6502
    emit_6502(f, fenv, venv); goto done;
#endif

    // really hacky #ifdef chain means I have to hop over this panic
    panic("Must define a supported architecture with -DARCH!\n");

    done:
    return;
}
