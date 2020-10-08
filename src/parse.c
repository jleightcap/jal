#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "parse.h"
#include "token.h"

struct token currtok;

#define checktok(tok, expect, str) \
    if(tok.type != expect) { \
        fprintf(stderr, "%s - didn't find expected token!\n", str); \
        exit(-1); \
    }

// convert a token holding a type to the type associated with a given name
enum type
typetok_to_type(const enum toktype t)
{
    switch(t) {
    case TYPE_INT:
        return INT;
    case TYPE_STR:
        return STRING;
    default:
        fprintf(stderr, "type not recognized!\n"); exit(-1);
    }
}

struct expr*
expr_init(void)
{
    return malloc(sizeof(struct expr));
}

void
expr_free(struct expr* e)
{
    switch(e->exptype) {
    case UNARY:
        expr_free(e->expression.unary.arg);
        break;
    case BINARY:
        expr_free(e->expression.binary.arg1);
        expr_free(e->expression.binary.arg2);
        break;
    case LITERAL:
        break;
    }
    free(e);
}

void
funenv_free(struct funenv* fenv)
{
    // iterate over whole function environment, most is probably empty
    for(unsigned int ii = 0; ii < ENV_SIZE; ii++) {
        // found a function; has expressions
        if(fenv->env[ii].exprs > 0) {
            // free all expressions associated with function
            for(unsigned int jj = 0; jj < fenv->env[ii].exprs; jj++) {
                expr_free(fenv->env[ii].body[jj]);
            }
        }
    }
}

void
varenv_free(struct varenv* venv)
{
    return;
}

// parse a expression.
// tokenizing begins with currtok at opening LPAREN
void
parse_expr(const unsigned long name, struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    switch(currtok.type) {
    // EXPRESSION
    case LPAREN:
        currtok = scan(); // operator
        switch(currtok.type) {
        // intrinsic binary ops
        case ADD:
            e->expression.binary.op = PLUS;
            goto intrinsic_binops;
        case SUB:
            e->expression.binary.op = MINUS;
            goto intrinsic_binops;
        case MUL:
            e->expression.binary.op = TIMES;
            goto intrinsic_binops;
        case DIV:
            e->expression.binary.op = DIVIDE;
            goto intrinsic_binops;
        default:
            fprintf(stderr, "Unsupported binary op!\n"); exit(-1);
        }
        intrinsic_binops:
            e->exptype = BINARY;
            // initialize arg1, associated with same parse arguments
            e->expression.binary.arg1 = expr_init();
            currtok = scan();
            parse_expr(name, e->expression.binary.arg1, fenv, venv);
            // initialize arg2, associated with same parse arguments
            e->expression.binary.arg2 = expr_init();
            currtok = scan();
            parse_expr(name, e->expression.binary.arg2, fenv, venv);

            currtok = scan();
            checktok(currtok, RPAREN, "intrinsic binary ops: too many arguments?");
            return;

    // ATOMS
    case NUM:
        e->exptype = LITERAL;
        e->expression.literal.litval.integer = currtok.value.num;
        return;
    case STR:
        e->exptype = LITERAL;
        memcpy(e->expression.literal.litval.string, currtok.value.str, MAX_STRLEN);
        return;

    // VARIABLE
    case SYM:
        // lookup symbol in variable environment
        e->exptype = LITERAL; // TODO: evaluate expression
        e->expression.literal.litval.integer = venv->env[currtok.value.hash].body.expression.literal.litval.integer;
        return;

    // RESERVED FUNCTIONS
    case PRINT:
        e->exptype = UNARY;
        e->expression.unary.op = I_PRINT;
        currtok = scan();
        parse_expr(name, e, fenv, venv);
        break;
    case RETRN:
        e->exptype = UNARY;
        e->expression.unary.op = I_RETRN;
        currtok = scan();
        parse_expr(name, e, fenv, venv);
        break;

    default:
        fprintf(stderr, "%d unexpected expression!\n", currtok.type); exit(-1);
    }

    currtok = scan();
    checktok(currtok, RPAREN, "expression end");
}

void
parse_devar(struct funenv* fenv, struct varenv* venv)
{
    // VARIABLE SIGNATURE PARSING
    currtok = scan(); // (
    checktok(currtok, LPAREN, "variable signature begin");
    currtok = scan(); // variable name
    const unsigned long name = currtok.value.hash;
    currtok = scan(); // variable type
    enum type t = typetok_to_type(currtok.type);
    currtok = scan(); // )
    checktok(currtok, RPAREN, "variable signature end");

    // VARIABLE BODY PARSING
    currtok = scan();
    struct expr* e = expr_init();
    parse_expr(name, e, fenv, venv);

    struct expr ans = eval(t, e, fenv, venv);
    expr_free(e);

    venv->env[name].type = t;
    venv->env[name].body = ans;

    currtok = scan();
    checktok(currtok, RPAREN, "variable defition end (match devar)");
}

// parse a function definition.
// tokenizing begins at left parentheses after "defun" token.
void
parse_defun(struct funenv* fenv, struct varenv* venv)
{
    // FUNCTION SIGNATURE PARSING
    currtok = scan(); // (
    checktok(currtok, LPAREN, "function signature begin");
    currtok = scan(); // function name
    const unsigned long name = currtok.value.hash;
    currtok = scan(); // function return type
    fenv->env[name].type = typetok_to_type(currtok.type);
    // TODO: this is where parsing arguments would occur!
    currtok = scan(); // )
    checktok(currtok, RPAREN, "function signature end");

    // FUNCTION BODY PARSING
    fenv->env[name].exprs = 0;
    for(currtok = scan(); currtok.type == LPAREN; currtok = scan()) {
        printf("expression %d\n", fenv->env[name].exprs);
        struct expr* e = (fenv->env[name].body[fenv->env[name].exprs] = expr_init());
        currtok = scan(); // function name
        parse_expr(name, e, fenv, venv);
        fenv->env[name].exprs += 1;
    }
    assert(fenv->env[name].exprs > 0 && "function must have a body!");
    checktok(currtok, RPAREN, "function body end");

    currtok = scan();
    checktok(currtok, RPAREN, "function declaration end (match defun)");
}

void
parse(struct funenv* fenv, struct varenv* venv)
{
    for(currtok = scan(); currtok.type != END; currtok = scan()) {
        checktok(currtok, LPAREN, "beginning expression");
        currtok = scan();

        // global definitions, can be:
        // - "defun"
        // - "defvar"
        switch(currtok.type) {
            // function definition
            case DEFUN:
                //printf("defun\n");
                parse_defun(fenv, venv);
                break;
            // variable definition
            case DEVAR:
                //printf("devar\n");
                parse_devar(fenv, venv);
                break;
            default:
                fprintf(stderr, "unexpected token at expression initialization!\n");
                exit(-1);
        }
    }
}
