#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "z80.h"
#include "eval.h"
#include "parse.h"
#include "util.h"

#define emit(str) fputs(str, _z80_out)
#define RAMSIZE 0xffff - 0x8000 // absolute addresses of jeighty RAM

typedef uint8_t word; // architecture word size

static word* _z80_ram;
static FILE* _z80_out;
static struct _z80_reg* _z80_reg;

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
    switch(f->ft) {
    case DEF: {
        // NOTE: this depends on the type used to hash function names!
        char namebuf[sizeof(unsigned long)];
        sprintf(namebuf, "%lx", f->name.hash);
        emit("func_"); emit(namebuf); emit(":\n");
        for(unsigned int ii = 0; ii < f->exprs; ii++) {
            emit_expr(f->body[ii], venv, fenv);
        }
        break;
    }
    case BUILTIN: {
        switch(f->name.b) {
        case F_ADD:
            assert(f->exprs == 2 &&
                   f->body[0]->exprtype == LITERAL &&
                   f->body[0]->e.lit.t == INT &&
                   f->body[1]->exprtype == LITERAL &&
                   f->body[1]->e.lit.t == INT &&
                   "TODO: support nested arithmetic");
            for(unsigned int ii = 0; ii < f->exprs; ii++) {
                emit_expr(f->body[ii], venv, fenv);
            }
            emit("\tadd\n");
            break;

        builtin_binop:
        case F_RET:
            for(unsigned int ii = 0; ii < f->exprs; ii++) {
                emit_expr(f->body[ii], venv, fenv);
            }
            emit("\tret\n");
            break;
        default:
            panic("TODO: Unhandled builtin function call");
        }
        break;
    }
    case CALL: {
        panic("TODO: call defined functions");
        break;
    }
    default:
        panic("Unhanlded function type in emit_func!");
        break;
    }
}

static void
emit_lit(struct lit const* l, struct varenv const* venv, struct funenv const* fenv)
{
    switch(l->t) {
        case INT: {
            char numbuf[50]; // if using more than 50 digits I don't care
            sprintf(numbuf, "%x", l->litval.integer);
            emit("\tld A, $"); emit(numbuf); emit("\n");
            break;
        }

        case STRING:
        case VOID:
        default: panic("unhandled type emit!");
    }
}

static void
emit_var(struct var const* v, struct varenv const* venv, struct funenv const* fenv)
{
    panic("TODO: emit_var");
}

void
emit_z80(FILE* f, struct funenv const* fenv, struct varenv const* venv)
{
    _z80_out = f;
    _z80_reg = calloc(1, sizeof(struct _z80_reg));
    _z80_ram = calloc(RAMSIZE, sizeof(word));

    // entrypoint
    emit("start:\n");
    char namebuf[sizeof(unsigned long)];
    const unsigned long mainhash = hashstr("main");
    sprintf(namebuf, "%lx", mainhash);
    emit("\tcall func_"); emit(namebuf); emit("\n");
    emit("\thalt\n\n");

    emit_func(&fenv->env[mainhash], venv, fenv);

    free(_z80_ram);
    free(_z80_reg);
}
