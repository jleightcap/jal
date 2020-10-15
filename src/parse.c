#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "parse.h"
#include "token.h"
#include "util.h"

struct token currtok;

unsigned int exprcount = 0;

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

void
var_free(struct var* v)
{
    lit_free(&v->lit);
}

struct expr*
expr_init(void)
{
    exprcount++;
    printf("exprinit %d\n", exprcount);
    return malloc(sizeof(struct expr));
}

void
expr_free(struct expr* e)
{
    exprcount--;
    printf("exprfree %d\n", exprcount);
    switch(e->exprtype) {
    case FUNCTION:
        func_free(&e->e.func);
        break;
    case LITERAL:
        lit_free(&e->e.lit);
        break;
    case VARIABLE:
        var_free(&e->e.var);
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
            func_free(&fenv->env[ii]);
        }
    }
}

void
varenv_free(struct varenv* venv)
{
    for(unsigned int ii = 0; ii < ENV_SIZE; ii++) {
        var_free(&venv->env[ii]);
    }
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
            /*
            e->e.func.t = VOID;
            e->e.func.ft = BUILTIN;
            e->e.func.name.b = F_DEVAR;
            e->e.func.exprs = 0;
            e->e.func.argnum = 1;
            parse_devar(fenv, venv);
            */
            panic("TODO: scoped devar!\n");
            break;
        case DEFUN:
            panic("defun not at top level!");
        case RET:
            //printf("ret\n");
            e->e.func.t = VOID;
            e->e.func.ft = BUILTIN;
            e->e.func.name.b = F_RET;
            e->e.func.argnum = 1;
            e->e.func.args.argt[0] = INT;
            e->e.func.exprs = 1;
            currtok = scan();
            parse_expr(e->e.func.body[0] = expr_init(), fenv, venv);
            checktok((currtok = scan()), RPAREN, "return end");
            break;
        case PRINT:
            //printf("print\n");
            e->e.func.t = VOID;
            e->e.func.ft = BUILTIN;
            e->e.func.name.b = F_PRINT;
            e->e.func.argnum = 0;
            e->e.func.exprs = 1;
            e->e.func.args.argt[0] = STRING;
            currtok = scan();
            parse_expr((e->e.func.body[0] = expr_init()), fenv, venv);
            currtok = scan();
            break;
        case QUINARY:
            //printf("quinary\n");
            e->e.func.ft = BUILTIN;
            e->e.func.name.b = F_QUI;
            // TODO: the consequence branch should be optional
            e->e.func.exprs = 3;
            struct expr* cond = (e->e.func.body[0] = expr_init());
            struct expr* resl = (e->e.func.body[1] = expr_init());
            struct expr* cons = (e->e.func.body[2] = expr_init());

            checktok((currtok = scan()), LPAREN, "quinary cond beginning");
            parse_quinary(cond, resl, cons, fenv, venv);
            checktok(currtok, RPAREN, "quinary cons end");

            enum type t_resl = (resl->exprtype == FUNCTION) ? resl->e.func.t : resl->e.lit.t;
            enum type t_cons = (cons->exprtype == FUNCTION) ? cons->e.func.t : cons->e.lit.t;
            assert(t_resl  == t_cons &&  "result and consequence must have same type!");
            e->e.func.t = t_resl; // quinary return type of each branch
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
            e->e.func.argnum = 0;
            e->e.func.exprs = 2;
            currtok = scan(); // begin first argument
            parse_expr((e->e.func.body[0] = expr_init()), fenv, venv);
            currtok = scan(); // begin second argument
            parse_expr((e->e.func.body[1] = expr_init()), fenv, venv);
            currtok = scan();
            break;

        // symbol preceded by LPAREN must be a function reference
        case SYM:
            //printf("func call\n");
            e->e.func.ft = CALL;
            assert(fenv->env[currtok.value.hash].body[0] != NULL
                   && "calling undefined function!");
            e->e.func.name.hash = fenv->env[currtok.value.hash].name.hash;
            e->e.func.argnum = 0;
            e->e.func.exprs = fenv->env[currtok.value.hash].exprs;
            currtok = scan();
            //printf("args = %d\n", e->e.func.argnum);
            for(unsigned int ii = 0; ii < e->e.func.exprs; ii++) {
                assert(currtok.type != RPAREN && "expected another argument!");
                e->e.func.body[ii] = expr_init();
                parse_expr(e->e.func.body[ii], fenv, venv);
                currtok = scan();
                //printf("parsed expr %d\n", ii);
            }
            checktok(currtok, RPAREN, "function arguments end");
            //printf("sym end %d\n", currtok.type);
            break;
        default:
            panic("unexpected function!");
        }
        break;

    // symbol not preceded by LPAREN must be a variable reference
    case SYM:
        e->exprtype = VARIABLE;
        e->e.var.hash = currtok.value.hash;
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
    checktok((currtok = scan()), RPAREN, "quinary cons expression end");
}

// parse a `devar` declaration
// parsing begins with currtok pointing to ( after 'devar'
// parsing ends with currtok pointing to ) after expression body
void
parse_devar(struct funenv* fenv, struct varenv* venv)
{
    // VARIABLE SIGNATURE PARSING
    checktok((currtok = scan()), LPAREN, "variable signature begin");
    currtok = scan(); // variable name
    const unsigned long name = currtok.value.hash;
    currtok = scan(); // variable type
    venv->env[name].t = typetok_to_type(currtok.type);
    checktok((currtok = scan()), RPAREN, "variable signature end");

    // VARIABLE BODY PARSING
    currtok = scan();
    struct expr* tmp = expr_init();
    parse_expr(tmp, fenv, venv);
    struct lit ans = eval(venv->env[name].t, tmp, fenv, venv);
    expr_free(tmp);

    venv->env[name].lit = ans;
    checktok((currtok = scan()), RPAREN, "variable body end");
}

// parse a function definition.
// tokenizing begins at left parentheses after "defun" token.
void
parse_defun(struct funenv* fenv, struct varenv* venv)
{
    // FUNCTION SIGNATURE PARSING
    checktok((currtok = scan()), LPAREN, "function signature begin");
    currtok = scan(); // function name
    const unsigned long name = currtok.value.hash;
    currtok = scan(); // function return type
    fenv->env[name].t = typetok_to_type(currtok.type);
    fenv->env[name].ft = DEF;
    fenv->env[name].name.hash = name;

    // ENVIRONMENT INHERITANCE
    // TODO: inheret parent venv and free accordingly
    /*
    fenv->env[name].venv = malloc(sizeof(struct varenv));
    memcpy(fenv->env[name].venv, venv, sizeof(struct varenv));
    */

    // FUNCTION ARGUMENT PARSING
    fenv->env[name].argnum = 0;
    for(currtok = scan(); currtok.type == LPAREN; currtok = scan()) {
        checktok(currtok, LPAREN, "function argument begin");
        unsigned int ii = fenv->env[name].argnum;
        currtok = scan(); // variable
        unsigned long argname = currtok.value.hash;
        fenv->env[name].args.arghash[ii] = argname; // map each argument to venv hash
        struct var* argvar = &venv->env[argname];
        currtok = scan(); // type
        argvar->t = typetok_to_type(currtok.type);
        fenv->env[name].argnum++;
        checktok((currtok = scan()), RPAREN, "function argument end");
        assert(ii < MAXARGS && "maximum function arguments, incrase MAXARGS!");
    }
    checktok(currtok, RPAREN, "function signature end");

    // FUNCTION BODY PARSING
    fenv->env[name].exprs = 0;
    for(currtok = scan(); currtok.type == LPAREN; currtok = scan()) {
        checktok(currtok, LPAREN, "function body expression begin");
        unsigned int ii = fenv->env[name].exprs;
        struct expr* e = (fenv->env[name].body[ii] = expr_init());
        parse_expr(e, fenv, venv);
        fenv->env[name].exprs++;
        checktok(currtok, RPAREN, "function body expression end");
        assert(ii < MAXEXPRS && "maximum function expressions, increase MAXEXPRS!");
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
            //printf("defun\n");
            parse_defun(fenv, venv);
            checktok(currtok, RPAREN, "top level DEFUN end");
            break;
        // variable definition
        case DEVAR:
            //printf("devar\n");
            parse_devar(fenv, venv);
            checktok(currtok, RPAREN, "top level DEVAR end");
            break;
        default:
            fprintf(stderr, "unexpected token %d at top level expression!\n", currtok.type);
            exit(-1);
        }
    }
}
