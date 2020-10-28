#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "z80.h"
#include "eval.h"
#include "parse.h"
#include "util.h"

#define emit(str) fputs(str, _z80_out)
#define RAMSIZE 0x3fff - 0x000

typedef uint8_t word; // architecture word size

static word* _z80_ram;
static FILE* _z80_out;
static struct _z80_reg_state* _z80_reg_state;

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
            emit("\tld, A $");
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
emit_z80(FILE* f, struct funenv const* fenv, struct varenv const* venv)
{
    _z80_out = f;
    _z80_reg_state = calloc(1, sizeof(struct _z80_reg_state));
    _z80_ram = calloc(RAMSIZE, sizeof(word));

    // entrypoint
    emit("start:\n");
    const unsigned long main = hashstr("main");
    emit_func(&fenv->env[main], venv, fenv);
    emit("\thalt\n");

    free(_z80_ram);
    free(_z80_reg_state);
}
