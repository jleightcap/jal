#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "6502.h"
#include "eval.h"
#include "parse.h"
#include "util.h"

FILE* _6502_out;

#define emit(str) fputs(str, _6502_out)

struct _6502_reg_state _6502_reg_state;

void
emit_6502(FILE* f, const struct funenv* fenv, const struct varenv* venv)
{
    _6502_out = f;
}
