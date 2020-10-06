#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"
#include "riscv64.h"
#include "parse.h"
#include "util.h"

FILE* out;

#define emit(str) fputs(str, out)

struct riscv64_reg_state reg_state;

// emit an available temporary register (t0-t6)
void
emit_tmp(void)
{
    // temporary registers are disjoint, loop over each set
    // (t0 - t2) and (t3 - t6)
    // DEPENDS: order of registers in enum riscv64_regs (riscv64.h)
    for(int tmp = T0; tmp <= T2; tmp++) {
        if(!reg_state.reg[tmp]) {
            switch(tmp) {
            case T0: emit("t0"); reg_state.reg[T0] = true; break;
            case T1: emit("t1"); reg_state.reg[T1] = true; break;
            case T2: emit("t2"); reg_state.reg[T2] = true; break;
            }
            return;
        }
    }
    for(int tmp = T3; tmp <= T6; tmp++) {
        if(!reg_state.reg[tmp]) {
            switch(tmp) {
            case T3: emit("t3"); reg_state.reg[T3] = true; break;
            case T4: emit("t4"); reg_state.reg[T4] = true; break;
            case T5: emit("t5"); reg_state.reg[T5] = true; break;
            }
            return;
        }
    }
    fprintf(stderr, "temporary registers full!\n"); exit(-1);
}

void
emit_expr(const struct expr* e, const struct funenv* fenv, const struct varenv* venv)
{
    char strbuf[100];
    switch(e->type) {
    case BINARY:
        emit_expr(e->expression.binary.arg1, fenv, venv);
        emit_expr(e->expression.binary.arg2, fenv, venv);
        switch(e->expression.binary.op) {
        case PLUS:
            emit("\tadd\ta0, t0, t1\n");
            break;
        case MINUS:
            emit("\tsub\ta0, t0, t1\n");
            break;
        case TIMES:
            emit("\tmul\ta0, t0, t1\n");
            break;
        case DIVIDE:
            emit("\tdiv\ta0, t0, t1\n");
            break;
        default:
            fprintf(stderr, "TODO: binary op\n"); exit(-1);
        }
        break;
    case LITERAL:
        sprintf(strbuf, "%d", e->expression.literal.litval.integer);
        emit("\taddi\t");
        // emit proper register
        emit_tmp();
        emit(", x0, ");
        emit(strbuf);
        emit("\n");
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
    memset(&reg_state.reg, 0, sizeof(struct riscv64_reg_state));
    
    emit("\t.globl _start\n");
    emit("_start:\n");

    // TODO: iterate over all expressions in main
    // here, just take the last expression (assumed to be the return)
    struct expr* mainexpr = fenv->env[main].body[0];

    // TODO: actual expression support!
    // - register allocation algorithms
    // - functions calls
    // - etc.
    //emit_expr(mainexp, fenv, venv);

    // TEMPORARY:
    char strbuf[100];
    struct expr ans = eval(INT, mainexpr, fenv, venv);
    sprintf(strbuf, "%d", ans.expression.literal.litval.integer);
    emit("\taddi\ta0, x0, ");
    emit(strbuf);
    emit("\n");
    // END

    emit("\taddi\ta7, x0, 93\n");
    reg_state.reg[A7] = true;
    emit("\tecall\n");
}
