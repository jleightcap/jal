#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "parse.h"
#include "util.h"

struct lit
eval_int(const struct expr* e, const struct funenv* fenv, const struct varenv* venv)
{
}

struct lit
eval(const enum type t, const struct expr* e,
     const struct funenv* fenv, const struct varenv* venv)
{
    struct lit lit;
    struct lit args[MAXARGS];
    lit.t = t;
    switch(e->exprtype) {
        case FUNCTION:
            switch(e->e.func.ft) {
            case BUILTIN:
                // TODO: this breaks if there any any non-binary builtin functions
                // would have to change, for example, if ADD supported arbitrary
                // arguments (which is not unreasonale)
                args[0] = eval(t, e->e.func.body[0], fenv, venv);
                args[1] = eval(t, e->e.func.body[1], fenv, venv);
                switch(e->e.func.name.b) {
                case F_ADD:
                    lit.litval.integer =
                        args[0].litval.integer + args[1].litval.integer;
                    break;
                case F_SUB:
                    lit.litval.integer =
                        args[0].litval.integer - args[1].litval.integer;
                    break;
                case F_MUL:
                    lit.litval.integer =
                        args[0].litval.integer * args[1].litval.integer;
                    break;
                case F_DIV:
                    lit.litval.integer =
                        args[0].litval.integer / args[1].litval.integer;
                    break;
                case F_MOD:
                    lit.litval.integer =
                        args[0].litval.integer % args[1].litval.integer;
                    break;
                }
                break;
            case TABLE:
                panic("TODO: eval functions!");
            }
            break;
        case LITERAL:
            lit = e->e.lit;
            break;
    }
    return lit;
}
