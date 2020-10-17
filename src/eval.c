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
        case DEF:
            panic("can't eval function definitions!");
        case BUILTIN:
            switch(e->e.func.name.b) {
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
            case F_QUI:
                assert(e->e.func.exprs == 3);
                if(args[0].litval.integer)
                    lit.litval.integer = args[1].litval.integer;
                else
                    lit.litval.integer = args[2].litval.integer;
                break;
            default:
                fprintf(stderr, "%d: ", e->e.func.name.b);
                panic("builtin not supported!\n");
            }
            break;
        case CALL:
        {
            struct func f = fenv->env[e->e.func.name.hash];
            print_defun(&f);
            // type-check, and populate function's arguments
            for(unsigned int ii = 0; ii < f.argnum; ii++) {
                unsigned long varhash = f.args.arghash[ii];
                enum type argt = f.args.argt[ii];

                struct lit argii = eval(argt, e->e.func.body[ii], fenv, venv);
                assert(argt == lit.t && "evaluating type mismatch!");

                // hmmm... I'm changing the global venv argument here...
                // if something goes wrong, look here pal :-)
                venv->env[varhash].lit = argii;
            }
            for(unsigned int ii = 0; ii < f.exprs; ii++) {
            }
            exit(1);
        }
        }
        break;
    case LITERAL:
        lit = e->e.lit;
        break;
    case VARIABLE:
        panic("TODO: variables in eval!");
    }
    return lit;
}
