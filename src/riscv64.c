#include <stdio.h>
#include <stdlib.h>

#include "riscv64.h"
#include "parse.h"
#include "util.h"

FILE* out;

#define emit(str) fputs(str, out)

void
emit_expr(struct expr* e, const struct funenv* fenv, const struct varenv* venv)
{
    char arg1[4], arg2[4];
    switch(e->type) {
    case BINARY:
        switch(e->body.binary.op) {
            case PLUS:
                sprintf(arg1, "%d", e->body.binary.arg1->body.val);
                emit("\taddi\tx1,x0,");
                emit(arg1);
                emit("\n");
                sprintf(arg2, "%d", e->body.binary.arg2->body.val);
                emit("\taddi\tx2,x0,");
                emit(arg2);
                emit("\n");
                emit("\tadd\ta0,x1,x2\n");
                break;
            default:
                fprintf(stderr, "TODO: binary op\n"); exit(-1);
        }
        break;
    default:
        fprintf(stderr, "TODO!\n"); exit(-1);
    }
}

void
emit_riscv64(FILE* f, const struct funenv* fenv, const struct varenv* venv)
{
    out = f;
    unsigned long main = hashstr("main");
    
    emit("\t.globl _start\n");
    emit("_start:\n");

    struct expr* mainexp = fenv->env[main].body;
    emit_expr(mainexp, fenv, venv);

    emit("\taddi\ta7,x0,93\n");
    emit("\tecall\n");

    /*
    char retstr[4];
    sprintf(retstr, "%d", ret);

    fprintf(out, "\t.globl _start\n");
    fprintf(out, "_start:\n");

    fprintf(out, "\taddi\ta0,x0,");
    fprintf(out, retstr);
    fprintf(out, "\n");

    fprintf(out, "\taddi\ta7,x0,93\n");
    fprintf(out, "\tecall\n");
    */
}
