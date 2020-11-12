#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "parse.h"
#include "util.h"

struct lit
eval(const enum type t, const struct expr* e,
     const struct funenv* fenv, struct varenv* venv)
{
    struct lit lit;
    struct lit args[MAXARGS];
    lit.t = t;

    switch(e->exprtype) {
    case FUNCTION:
        for(unsigned int ii = 0; ii < e->e.func.exprs; ii++)
            args[ii] = eval(t, e->e.func.body[ii], fenv, venv);
        switch(e->e.func.ft) {
        case FT_DEF:
            panic("can't eval function definitions!");
        case FT_BUILTIN:
            switch(e->e.func.name.b) {
            // arithmetic
            case F_ADD:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer + args[1].litval.integer;
                break;
            case F_SUB:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer - args[1].litval.integer;
                break;
            case F_MUL:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer * args[1].litval.integer;
                break;
            case F_DIV:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer / args[1].litval.integer;
                break;
            case F_MOD:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer % args[1].litval.integer;
                break;
            // binary operators
            case F_NOT:
                assert(e->e.func.exprs == 1);
                lit.litval.integer =
                    ~(args[0].litval.integer);
                break;
            case F_AND:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer & args[1].litval.integer;
                break;
            case F_OR:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer | args[1].litval.integer;
                break;
            case F_LSL:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer << args[1].litval.integer;
                break;
            case F_LSR:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer >> args[1].litval.integer;
                break;
            // comparators
            case F_EQ:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer == args[1].litval.integer;
                break;
            case F_NE:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer != args[1].litval.integer;
                break;
            case F_GT:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer > args[1].litval.integer;
                break;
            case F_LT:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer < args[1].litval.integer;
                break;
            case F_GE:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer >= args[1].litval.integer;
                break;
            case F_LE:
                assert(e->e.func.exprs == 2);
                lit.litval.integer =
                    args[0].litval.integer <= args[1].litval.integer;
                break;
            // special function tokens
            case F_INC:
            case F_DEC:
                panic("TODO: assignment in operator"); break;
            case F_QUI:
                assert(e->e.func.exprs == 3);
                if(args[0].litval.integer)
                    lit.litval.integer = args[1].litval.integer;
                else
                    lit.litval.integer = args[2].litval.integer;
                break;
            case F_ASSGN:
                panic("TODO: assignment in operator"); break;
            default:
                fprintf(stderr, "%d: ", e->e.func.name.b);
                panic("builtin not supported!\n");
            }
            break;
        case FT_CALL:
        {
            struct func f = fenv->env[e->e.func.name.hash];
            // type-check, and populate function's arguments
            assert(f.argnum == e->e.func.argnum && "incorrect number of arguments!");
            for(unsigned int ii = 0; ii < f.argnum; ii++) {
                unsigned long varhash = f.args.arghash[ii];
                enum type argt = f.args.argt[ii];

                struct lit argii = eval(argt, e->e.func.body[ii], fenv, venv);
                assert(argt == lit.t && "evaluating type mismatch!");

                // hmmm... I'm changing the global venv argument here...
                // if something goes wrong with variables, look here pal :-)
                venv->env[varhash].lit = argii;
            }
            for(unsigned int ii = 0; ii < f.exprs; ii++) {
                // unwrap Matryoshka-style for special return case
                if(f.body[ii]->exprtype == FUNCTION &&
                   f.body[ii]->e.func.ft == FT_BUILTIN &&
                   f.body[ii]->e.func.name.b == F_RET)
                {
                    assert(f.body[ii]->e.func.exprs == 1 && "expected 1 return expression!");
                    lit = eval(f.t, f.body[ii]->e.func.body[0], fenv, venv);
                    printf("evaluated literal: ");
                    print_lit(&lit, 0);
                }
                else {
                    // not 'return': the expression may change the environment,
                    // but discard the literal result
                    eval(f.t, f.body[ii], fenv, venv);
                }
            }
            break;
        case FT_IMPORT:
            panic("can't eval an import expression!");
        }
        }
        break;
    case LITERAL:
        lit = e->e.lit;
        break;
    case VARIABLE:
        lit = venv->env[e->e.var.hash].lit;
        break;
    default:
        panic("unhandled expression type in eval!");
    }
    return lit;
}
