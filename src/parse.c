#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "parse.h"
#include "token.h"
#include "util.h"

struct token currtok;

void parse_devar(struct funenv* fenv, struct varenv* venv);
void parse_defun(struct funenv* fenv, struct varenv* venv);
void parse_expr(const unsigned long name, struct expr* e, struct funenv* fenv, struct varenv* venv);
void parse(struct funenv* fenv, struct varenv* venv);

#define checktok(tok, expect, str) \
    if(tok.type != expect) { \
        fprintf(stderr, "%s - didn't find expected token!\n", str); \
        exit(-1); \
    }

void
func_free(struct func* f)
{
    for(unsigned int ii = 0; ii < f->exprs; ii++) {
        expr_free(f->body[ii]);
    }
}

void
lit_free(struct lit* v)
{
    return;
}

struct expr*
expr_init(void)
{
    return malloc(sizeof(struct expr));
}

void
expr_free(struct expr* e)
{
    switch(e->exprtype) {
    case FUNCTION:
        func_free(&e->e.func);
        break;
    case LITERAL:
        lit_free(&e->e.lit);
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

// parse an expression from tokens, populating the expression e.
// parsing begins with currtok at first token of expression
// parsing ends with currtok at token after last token of expression
void
parse_expr(const unsigned long name, struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    switch(currtok.type) {
    // LPAREN must be a function call
    case LPAREN:
        e->exprtype = FUNCTION;
        currtok = scan(); // operator
        switch(currtok.type) {
        // resrved symbols
        case DEVAR:
            //printf("inner devar\n");
            e->e.func.t = VOID;
            e->e.func.ft = TABLE;
            e->e.func.name.hash = currtok.value.hash;
            parse_devar(fenv, venv);
            break;
        case DEFUN:
            panic("Defun not at top level!");
            break;
        case RET:
            //printf("ret\n");
            e->e.func.t = VOID;
            e->e.func.ft = TABLE;
            e->e.func.name.hash = currtok.value.hash;
            e->e.func.exprs = 1;
            currtok = scan();
            parse_expr(name, e->e.func.body[0] = expr_init(), fenv, venv);
            break;
        case PRINT:
            //printf("print\n");
            e->e.func.t = VOID;
            e->e.func.ft = TABLE;
            e->e.func.name.hash = currtok.value.hash;
            e->e.func.exprs = 1;
            currtok = scan();
            parse_expr(name, e->e.func.body[0] = expr_init(), fenv, venv);
            break;

        case ADD:
            e->e.func.name.b = F_ADD; goto builtin_binops;
        case SUB:
            e->e.func.name.b = F_SUB; goto builtin_binops;
        case MUL:
            e->e.func.name.b = F_MUL; goto builtin_binops;
        case DIV:
            e->e.func.name.b = F_DIV; goto builtin_binops;
        builtin_binops:
            e->e.func.t = INT;
            e->e.func.ft = BUILTIN;
            e->e.func.exprs = 2;
            currtok = scan(); // begin first argument
            parse_expr(name, (e->e.func.body[0] = expr_init()), fenv, venv);
            currtok = scan(); // begin second argument
            parse_expr(name, (e->e.func.body[1] = expr_init()), fenv, venv);
            currtok = scan();
            break;

        case SYM:
            panic("TODO: defun lookups!");
        default:
            panic("unexpected function!");
        }
        break;

    // symbol not preceded by LPAREN must be a variable reference
    case SYM:
        e->exprtype = LITERAL;
        assert(venv->env[currtok.value.hash].body != NULL && "variable does not exist in this scope!");
        struct lit lit = venv->env[currtok.value.hash].body->e.lit;
        e->e.lit.t = lit.t;
        switch(lit.t) {
        case INT:
            e->e.lit.litval.integer = lit.litval.integer;
            break;
        case STRING:
            memcpy(e->e.lit.litval.string, lit.litval.string, MAX_STRLEN);
            break;
        case VOID:
            panic("cannot reference a void type!");
        }
        break;

    // atoms
    case A_INT:
        e->exprtype = LITERAL;
        e->e.lit.t = INT;
        e->e.lit.litval.integer = currtok.value.num;
        break;
    case A_STR:
        e->exprtype = LITERAL;
        e->e.lit.t = STRING;
        memcpy(e->e.lit.litval.string, currtok.value.str, MAX_STRLEN + 1);
        break;

    default:
        fprintf(stderr, "unexpected expression token!\n"); exit(-1);
    }
}

// parse a `devar` declaration
// parsing begins with currtok pointing to ( after 'devar'
// parsing ends with currtok pointing to ) after expression body
void
parse_devar(struct funenv* fenv, struct varenv* venv)
{
    // VARIABLE SIGNATURE PARSING
    currtok = scan(); // (
    checktok(currtok, LPAREN, "variable signature begin");
    currtok = scan(); // variable name
    const unsigned long name = currtok.value.hash;
    currtok = scan(); // variable type
    venv->env[name].t = typetok_to_type(currtok.type);
    currtok = scan(); // )
    checktok(currtok, RPAREN, "variable signature end");

    // VARIABLE BODY PARSING
    currtok = scan();
    struct expr* e = (venv->env[name].body = expr_init());
    parse_expr(name, e, fenv, venv);

    // Evaluate body to a literal type
    struct lit ans = eval(venv->env[name].t, e, fenv, venv);
    venv->env[name].body->e.lit = ans;
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
    fenv->env[name].t = typetok_to_type(currtok.type);
    fenv->env[name].ft = TABLE;
    fenv->env[name].name.hash = name;
    // TODO: this is where parsing arguments would occur!
    currtok = scan(); // )
    checktok(currtok, RPAREN, "function signature end");

    // FUNCTION BODY PARSING
    fenv->env[name].exprs = 0;
    for(currtok = scan(); currtok.type == LPAREN; currtok = scan()) {
        unsigned int ii = fenv->env[name].exprs;
        fenv->env[name].body[ii] = expr_init();
        struct expr* e = fenv->env[name].body[ii];
        parse_expr(name, e, fenv, venv);
        currtok = scan();
        fenv->env[name].exprs += 1;
    }
    //printf("%d exprs: at end of defun: %d\n", fenv->env[name].exprs,  currtok.type);
    assert(fenv->env[name].exprs > 0 && "function must have a body!");

    //checktok(currtok, RPAREN, "function declaration end (match defun)");
}

void
parse(struct funenv* fenv, struct varenv* venv)
{
    for(currtok = scan(); currtok.type != END; currtok = scan()) {
        checktok(currtok, LPAREN, "beginning top level expression");
        currtok = scan();

        // global definitions, can be:
        // - "defun"
        // - "defvar"
        switch(currtok.type) {
            // function definition
            case DEFUN:
                //printf("defun\n");
                parse_defun(fenv, venv);
                checktok(currtok, RPAREN, "top level DEFUN end");
                break;
            // variable definition
            case DEVAR:
                //printf("devar\n");
                parse_devar(fenv, venv);
                currtok = scan();
                checktok(currtok, RPAREN, "top level DEVAR end");
                break;
            default:
                fprintf(stderr, "unexpected token at top level expression!\n");
                exit(-1);
        }
    }
}
