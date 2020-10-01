#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "riscv64.h"
#include "parse.h"
#include "util.h"

FILE* out;

#define emit(str) fputs(str, out)

struct riscv64_reg_state reg_state;

void
emit_expr(struct expr* e, const struct funenv* fenv, const struct varenv* venv)
{
    char strbuf[100];
    switch(e->type) {
    case BINARY:
        emit_expr(e->body.binary.arg1, fenv, venv);
        emit_expr(e->body.binary.arg2, fenv, venv);
        switch(e->body.binary.op) {
        case PLUS:
            emit("\tadd\ta0, t0, t1\n");
            break;
        case MINUS:
            emit("\tsub\ta0, t0, t1\n");
            break;
        default:
            fprintf(stderr, "TODO: binary op\n"); exit(-1);
        }
        break;
    case LITERAL_INT:
        emit("\taddi\tt0 , x0, ");
        sprintf(strbuf, "%d", e->body.val);
        emit(strbuf);
        emit("\n");
        if(reg_state.reg[T0])
            printf("already used!\n");
        reg_state.reg[T0] = true;
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
    memset(&reg_state, 0, sizeof(struct riscv64_reg_state));
    
    emit("\t.globl _start\n");
    emit("_start:\n");

    struct expr* mainexp = fenv->env[main].body;
    emit_expr(mainexp, fenv, venv);

    emit("\taddi\ta7, x0, 93\n");
    reg_state.reg[A7] = true;
    emit("\tecall\n");
}
