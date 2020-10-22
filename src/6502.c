#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "6502.h"
#include "eval.h"
#include "parse.h"
#include "util.h"

#define emit(str) fputs(str, _6502_out)
#define RAMSIZE 0x3fff - 0x000

typedef uint8_t word; // architecture word size

static word* _6502_ram;
static FILE* _6502_out;
static struct _6502_reg_state* _6502_reg_state;

static void emit_expr(struct expr const* e,
                      struct varenv const* venv, struct funenv const* fenv);
static void emit_func(struct func const* f,
                      struct varenv const* venv, struct funenv const* fenv);
static void emit_lit(struct lit const* l,
                     struct varenv const* venv, struct funenv const* fenv);
static void emit_var(struct var const* v,
                     struct varenv const* venv, struct funenv const* fenv);

static void
emit_expr(struct expr const* e, struct varenv const* venv, struct funenv const* fenv)
{
    //print_expr(e, 0);
    switch(e->exprtype) {
    case FUNCTION: { emit_func(&e->e.func, venv, fenv); break; }
    case LITERAL:  { emit_lit(&e->e.lit, venv, fenv);   break; }
    case VARIABLE: { emit_var(&e->e.var, venv, fenv);   break; }
    default: panic("unhandled expression type in emit_expr!");
    }
}

static void
emit_func(struct func const* f, struct varenv const* venv, struct funenv const* fenv)
{
    for(unsigned int ii = 0; ii < f->exprs; ii++) {
        emit_expr(f->body[ii], venv, fenv);
    }
}

static void
emit_lit(struct lit const* l, struct varenv const* venv, struct funenv const* fenv)
{
    char numbuf[50];
    switch(l->t) {
        case INT:
            sprintf(numbuf, "%x", l->litval.integer);
            emit("\tlda #$");
            emit(numbuf);
            emit("\n");
            break;

        case STRING:
        case VOID:
        default: panic("unhandled type emit!");
    }
}

static void
emit_var(struct var const* v, struct varenv const* venv, struct funenv const* fenv)
{
    panic("TODO: emit_lit");
}

void
emit_6502(FILE* f, struct funenv const* fenv, struct varenv const* venv)
{
    _6502_out = f;
    _6502_reg_state = calloc(1, sizeof(struct _6502_reg_state));
    _6502_ram = calloc(RAMSIZE, sizeof(word));

    emit("\torg $8000\n"); // assemble to ROM

    // entrypoint
    emit("\nstart:\n");
    const unsigned long main = hashstr("main");
    emit_func(&fenv->env[main], venv, fenv);
    emit("\tbrk\n");

    free(_6502_ram);
    free(_6502_reg_state);
}
