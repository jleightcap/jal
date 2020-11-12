#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "emit.h"
#include "util.h"

// architectures
#include "riscv64.h"
#include "z80.h"

#ifdef _Z80
void
emit_init(FILE* f)
{
    z80_emit_init(f);
}

void
emit_free(void)
{
    z80_emit_free();
}

void
emit(const struct funenv* fenv, const struct varenv* venv)
{
    z80_emit(fenv, venv);
}

void
emit_include(const char* path)
{
    char* asmpath = malloc(MAX_STRLEN * sizeof(char));
    // take everything before the '.' in the path, and replace suffix with .asm
    unsigned long ii;
    for(ii = 0; ii < MAX_STRLEN; ii++) {
        if(path[ii] != '.') continue;
        else break;
    }
    memcpy(asmpath, path, ii);
    char suffix[5] = ".asm";
    memcpy(asmpath + ii, suffix, 5);
    z80_emit_include(asmpath);
}
#endif

#ifdef _RISCV_64
void
emit_init(FILE* f)
{
}

void
emit(const struct funenv* fenv, const struct varenv* venv)
{
    emit_riscv64(fenv, venv);
}
#endif
