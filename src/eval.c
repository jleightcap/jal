#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "parse.h"

#define typeassert(expr, t) \
    assert(expr.type == t && "type mismatch!");

struct expr
eval(const enum exprtype t, const struct expr* e,
     const struct funenv* fenv, const struct varenv* venv)
{
    struct expr ans;
    struct expr arg1;
    struct expr arg2;

    ans.type = t;
    switch(e->type) {
    case UNARY:
        fprintf(stderr, "TODO: unary expression eval\n"); exit(-1);
        break;
    case BINARY:
        arg1 = eval(t, e->body.binary.arg1, fenv, venv);
        arg2 = eval(t, e->body.binary.arg2, fenv, venv);
        switch(e->body.binary.op) {
            case PLUS:
                typeassert(ans, LITERAL_INT);
                ans.body.val = arg1.body.val + arg2.body.val;
                break;
            case MINUS:
                assert(ans.type == LITERAL_INT && "adding not integers!");
                ans.body.val = arg1.body.val - arg2.body.val;
                break;
            case TIMES:
                assert(ans.type == LITERAL_INT && "adding not integers!");
                ans.body.val = arg1.body.val * arg2.body.val;
                break;
            case DIVIDE:
                assert(ans.type == LITERAL_INT && "adding not integers!");
                ans.body.val = arg1.body.val / arg2.body.val;
                break;
        }
        break;
    case LITERAL_INT:
        ans.body.val = e->body.val;
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
        switch(e->body.binary.op) {
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
        print_expr(e->body.binary.arg1, nest + 1);
        print_expr(e->body.binary.arg2, nest + 1);
        break;
    case LITERAL_INT:
        printf("%d", e->body.val);
        printf(" :litint\n");
        break;
    }
}
