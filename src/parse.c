#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "parse.h"
#include "token.h"
#include "util.h"

struct token currtok;

// parse functions' signatures
void parse_devar(struct funenv* fenv, struct varenv* venv);
void parse_defun(struct funenv* fenv, struct varenv* venv);
void parse_expr(struct expr* e, struct funenv* fenv, struct varenv* venv);
void parse_quinary(struct expr* cond, struct expr* resl, struct expr* cons,
                   struct funenv* fenv, struct varenv* venv);

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
parse_expr(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    //printf("currtok type %d\n", currtok.type);
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
            e->e.func.ft = BUILTIN;
            e->e.func.name.b = F_DEVAR;
            parse_devar(fenv, venv);
            break;
        case DEFUN:
            panic("defun not at top level!");
            break;
        case RET:
            //printf("ret\n");
            e->e.func.t = VOID;
            e->e.func.ft = BUILTIN;
            e->e.func.name.b = F_RET;
            e->e.func.exprs = 1;
            currtok = scan();
            parse_expr(e->e.func.body[0] = expr_init(), fenv, venv);
            currtok = scan();
            checktok(currtok, RPAREN, "return end");
            break;
        case PRINT:
            //printf("print\n");
            e->e.func.t = VOID;
            e->e.func.ft = BUILTIN;
            e->e.func.name.b = F_PRINT;
            e->e.func.exprs = 1;
            currtok = scan();
            parse_expr(e->e.func.body[0] = expr_init(), fenv, venv);
            currtok = scan();
            break;
        case QUINARY:
            printf("quinary\n");
            e->e.func.t = VOID;
            e->e.func.ft = BUILTIN;
            e->e.func.name.b = F_QUI;
            // TODO: the consequence branch should be optional
            e->e.func.exprs = 3;
            struct expr* cond = (e->e.func.body[0] = expr_init());
            struct expr* resl = (e->e.func.body[1] = expr_init());
            struct expr* cons = (e->e.func.body[2] = expr_init());
            currtok = scan();
            checktok(currtok, LPAREN, "quinary cond beginning");
            parse_quinary(cond, resl, cons, fenv, venv);
            checktok(currtok, RPAREN, "quinary cons end");
            break;

        case ADD: e->e.func.name.b = F_ADD; goto builtin_binops;
        case SUB: e->e.func.name.b = F_SUB; goto builtin_binops;
        case MUL: e->e.func.name.b = F_MUL; goto builtin_binops;
        case DIV: e->e.func.name.b = F_DIV; goto builtin_binops;
        case MOD: e->e.func.name.b = F_MOD; goto builtin_binops;
        case EQ:  e->e.func.name.b = F_EQ;  goto builtin_binops;
        case NE:  e->e.func.name.b = F_NE;  goto builtin_binops;
        case GT:  e->e.func.name.b = F_GT;  goto builtin_binops;
        case LT:  e->e.func.name.b = F_LT;  goto builtin_binops;
        case GE:  e->e.func.name.b = F_GE;  goto builtin_binops;
        case LE:  e->e.func.name.b = F_LE;  goto builtin_binops;

        builtin_binops:
            e->e.func.t = INT;
            e->e.func.ft = BUILTIN;
            e->e.func.exprs = 2;
            currtok = scan(); // begin first argument
            parse_expr((e->e.func.body[0] = expr_init()), fenv, venv);
            currtok = scan(); // begin second argument
            parse_expr((e->e.func.body[1] = expr_init()), fenv, venv);
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
        fprintf(stderr, "unexpected expression token %d!\n", currtok.type);
        exit(-1);
    }
}

// parse a quinary expression
// parsing begins with currtok pointint to ( after ?
// parsing ends with currtok pointing to ) matching quinary declaration
void
parse_quinary(struct expr* cond, struct expr* resl, struct expr* cons,
              struct funenv* fenv, struct varenv* venv)
{
    checktok(currtok, LPAREN, "quinary cond expression begin");
    parse_expr(cond, fenv, venv);
    currtok = scan();
    parse_expr(resl, fenv, venv);
    currtok = scan();
    parse_expr(cons, fenv, venv);
    currtok = scan();
    checktok(currtok, RPAREN, "quinary cons expression end");
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
    parse_expr(e, fenv, venv);

    // Evaluate body to a literal type
    struct lit ans = eval(venv->env[name].t, e, fenv, venv);
    venv->env[name].body->e.lit = ans;

    currtok = scan();
    checktok(currtok, RPAREN, "variable body end");
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
        checktok(currtok, LPAREN, "function body expression begin");
        unsigned int ii = fenv->env[name].exprs;
        struct expr* e = (fenv->env[name].body[ii] = expr_init());
        parse_expr(e, fenv, venv);
        fenv->env[name].exprs += 1;
        checktok(currtok, RPAREN, "function body expression end");
        printf("parsed function body expression %d\n", ii);
    }
    assert(fenv->env[name].exprs > 0 && "function must have a body!");
    checktok(currtok, RPAREN, "function declaration end (match defun)");
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
            printf("defun\n");
            parse_defun(fenv, venv);
            checktok(currtok, RPAREN, "top level DEFUN end");
            break;
        // variable definition
        case DEVAR:
            printf("devar\n");
            parse_devar(fenv, venv);
            checktok(currtok, RPAREN, "top level DEVAR end");
            break;
        default:
            fprintf(stderr, "unexpected token %d at top level expression!\n", currtok.type);
            exit(-1);
        }
    }
}
