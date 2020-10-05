#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "parse.h"

#define typeassert(expr, type) \
    assert(expr.expression.literal.t == type && "type mismatch!");

#define MAXARGS 10

struct expr
eval(const enum type t, const struct expr* e,
     const struct funenv* fenv, const struct varenv* venv)
{
    struct expr ans;
    struct expr args[MAXARGS];
    int argval[MAXARGS];

    ans.expression.literal.t = t;
    switch(e->type) {
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

// print a nice looking indent
#define indt(x) \
    for(int ii = 0; ii < 2*x; ii++) \
        printf(" "); \
    printf("\033[1m\033[32m|-\033[m");

void
print_expr(const struct expr* e, int nest)
{
    indt(nest);
    switch(e->type) {
    case UNARY:
        break;
    case BINARY:
        switch(e->expression.binary.op) {
        case PLUS:
            printf("'+'");
            break;
        case MINUS:
            printf("'-'");
            break;
        case TIMES:
            printf("'*'");
            break;
        case DIVIDE:
            printf("'/'");
            break;
        }
        printf(" :binop\n");
        print_expr(e->expression.binary.arg1, nest + 1);
        print_expr(e->expression.binary.arg2, nest + 1);
        break;
    case LITERAL:
        // TODO: switch on literal types
        printf("%d", e->expression.literal.litval.integer);
        printf(" :litint\n");
        break;
    }
}
