#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "parse.h"

/*
#define typeassert(expr, type) \
    assert(expr.expression.literal.t == type && "type mismatch!");


struct expr
eval(const enum type t, const struct expr* e,
     const struct funenv* fenv, const struct varenv* venv)
{
    struct expr ans;
    struct expr args[MAXARGS];
    int argval[MAXARGS];

    ans.expression.literal.t = t;
    switch(e->exptype) {
    case UNARY:
        fprintf(stderr, "TODO: unary expression eval\n"); exit(-1);
        break;
    case BINARY:
        args[0] = eval(t, e->expression.binary.arg1, fenv, venv);
        args[1] = eval(t, e->expression.binary.arg2, fenv, venv);
        switch(e->expression.binary.op) {
            case PLUS:
                typeassert(ans, INT);
                argval[0] = args[0].expression.literal.litval.integer;
                argval[1] = args[1].expression.literal.litval.integer;
                ans.expression.literal.litval.integer = argval[0] + argval[1];
                break;
            case MINUS:
                typeassert(ans, INT);
                argval[0] = args[0].expression.literal.litval.integer;
                argval[1] = args[1].expression.literal.litval.integer;
                ans.expression.literal.litval.integer = argval[0] - argval[1];
                break;
            case TIMES:
                typeassert(ans, INT);
                argval[0] = args[0].expression.literal.litval.integer;
                argval[1] = args[1].expression.literal.litval.integer;
                ans.expression.literal.litval.integer = argval[0] * argval[1];
                break;
            case DIVIDE:
                typeassert(ans, INT);
                argval[0] = args[0].expression.literal.litval.integer;
                argval[1] = args[1].expression.literal.litval.integer;
                ans.expression.literal.litval.integer = argval[0] / argval[1];
                break;
        }
        break;
    case LITERAL:
        ans.expression.literal.litval.integer = e->expression.literal.litval.integer;
        break;
    }

    return ans;
}
*/

