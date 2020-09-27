#include <stdio.h>

#include "riscv64.h"
#include "parse.h"
#include "util.h"

FILE* out;

void
emit_riscv64(FILE* f, const struct funenv* fenv, const struct varenv* venv)
{
    out = f;

    unsigned long main = hashstr("main");
    int ret = fenv->env[main].body.val;
    char retstr[4];
    sprintf(retstr, "%d", ret);

    fprintf(out, "\t.globl _start\n");
    fprintf(out, "_start:\n");

    fprintf(out, "\taddi\ta0,x0,");
    fprintf(out, retstr);
    fprintf(out, "\n");

    fprintf(out, "\taddi\ta7,x0,93\n");
    fprintf(out, "\tecall\n");
}
