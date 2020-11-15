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
static struct _z80_reg_state* _z80_reg_state;

// private function signatures
static void
emit_expr(struct expr const*, struct varenv const*, struct funenv const*);
static void
emit_func(struct func const*, struct varenv const*, struct funenv const*);
static void
emit_lit(struct lit const*, struct varenv const*, struct funenv const*);
static void
emit_var(struct var const*, struct varenv const*, struct funenv const*);

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
    case FT_DEF: {
        emit(f->name.fname); emit(":\n");
        for(unsigned int ii = 0; ii < f->exprs; ii++) {
            emit_expr(f->body[ii], venv, fenv);
        }
        if(f->t == VOID) {
            emit("\tret\n");
        }
        break;
    }
    case FT_BUILTIN: {
        switch(f->name.b) {
        // accumulating binary operators
        case F_ADD: case F_SUB: case F_MUL: case F_DIV: case F_MOD:
            assert(f->exprs == 2 && "TODO: variadic arithmetic");
            // combo of local and global variable seems redundant?
            // want to initialize accumulator once per top-level arithmetic
            // expression and this seems the most straightforward way
            bool rootexpr = false;
            if(_z80_reg_state->A == false) {
                emit("\tld A, 0\n");
                _z80_reg_state->A = true; rootexpr = true;
            }
            for(unsigned int ii = 0; ii < f->exprs; ii++) {
                switch(f->body[ii]->exprtype) {
                case FUNCTION:
                    assert(f->body[ii]->e.func.t == INT
                           && "Unexpected non-arithmetic function!");
                    emit_func(&f->body[ii]->e.func, venv, fenv);
                    break;
                case LITERAL:
                    assert(f->body[ii]->e.lit.t == INT
                           && "Unexpected literal in arithmetic expression!");
                    switch(f->name.b) {
                    case F_ADD: {
                        emit("\tadd A, "); break;
                    }
                    case F_SUB: { // non-commutative
                        switch(ii) {
                        case 0: emit("\tadd A, "); break;
                        case 1: emit("\tsub ");    break;
                        default: panic("OOB sub arguments!");
                        }
                        break;
                    }
                    case F_MUL:
                    case F_DIV:
                    case F_MOD:
                    default:
                        panic("TODO: non-simple accumulating binary operators");
                    }
                    emit_lit(&f->body[ii]->e.lit, venv, fenv);
                    emit("\n");
                    break;
                case VARIABLE:
                    emit_var(&f->body[ii]->e.var, venv, fenv);
                    break;
                }
            }
            // de-initialize accumulator
            if(rootexpr == true) {
                _z80_reg_state->A = false; rootexpr = false;
            }
            break;

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
    case FT_CALL: {
        panic("TODO: call defined function");
    }
    case FT_IMPORT: {
        for(unsigned int ii = 0; ii < f->argnum; ii++) {
            assert(f->body[ii]->exprtype == LITERAL &&
                   f->body[ii]->e.lit.t == INT &&
                   "TODO: non-int arguments in import call");
            emit("\tld hl, "); emit_expr(f->body[ii], venv, fenv); emit("\n");
            emit("\tpush hl\n");
        }
        emit("\tcall "); emit(f->name.fname); emit("\n");
        break;
    }
    }
}

static void
emit_lit(struct lit const* l, struct varenv const* venv, struct funenv const* fenv)
{
    switch(l->t) {
        case INT: {
            char numbuf[50]; // if using more than 50 digits I don't care
            sprintf(numbuf, "%d", l->litval.integer);
            emit(numbuf);
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
z80_emit_include(const char* path)
{
    emit("include '"); emit(path); emit("'\n\n");
}

void
z80_emit(const struct funenv* fenv, const struct varenv* venv)
{
    // because emitting every function as ordered by hash,
    // first instructions are to jump to main entrypoint.
    //
    // other strategy is to emit the main function, and then
    // recursively only the functions reachable from main.
    
    // go through entire function environment, emmiting
    // any entries that have an associated body.
    for(unsigned int ii = 0; ii < ENV_SIZE; ii++) {
        // found a function; has expressions
        if(fenv->env[ii].exprs > 0) {
            emit_func(&fenv->env[ii], venv, fenv);
        }
    }
}

void
z80_emit_init(FILE* f)
{
    _z80_out = f;
    // use malloc for unintialized memory valgrind warnings
    _z80_reg = malloc(sizeof(struct _z80_reg));
    // RAM and register states are initially 0, can use uninitialized
    _z80_reg_state = calloc(1, sizeof(struct _z80_reg_state));
    _z80_ram = calloc(RAMSIZE, sizeof(word));
}

void
z80_emit_free(void)
{
    free(_z80_ram);
    free(_z80_reg_state);
    free(_z80_reg);
}
