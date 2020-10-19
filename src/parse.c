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

// parse function signatures
void parse_expr(struct expr* e, struct funenv* fenv, struct varenv* venv);
void parse_defun(struct funenv* fenv, struct varenv* venv);
void parse_devar(struct funenv* fenv, struct varenv* venv);
void parse_ret(struct expr* e, struct funenv* fenv, struct varenv* venv);
void parse_print(struct expr* e, struct funenv* fenv, struct varenv* venv);
void parse_quinary(struct expr* e, struct funenv* fenv, struct varenv* venv);
void parse_while(struct expr* e, struct funenv* fenv, struct  varenv* venv);
void parse_assign(struct expr* e, struct funenv* fenv, struct varenv* venv);
void parse_binop(struct expr* e, struct funenv* fenv, struct varenv* venv);
void parse_call(struct expr* e, struct funenv* fenv, struct varenv* venv);

struct expr*
expr_init(void)
{
    exprcount++;
    //printf("exprinit %d\n", exprcount);
    return malloc(sizeof(struct expr));
}

void
func_free(struct func* f)
{
    switch(f->ft) {
    case DEF:
        for(unsigned int ii = 0; ii < f->exprs; ii++) {
            expr_free(f->body[ii]);
        }
        free(f->venv);
        break;
    case BUILTIN:
    case CALL:
        for(unsigned int ii = 0; ii < f->argnum; ii++) {
            expr_free(f->body[ii]);
        }
        break;
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

void
expr_free(struct expr* e)
{
    exprcount--;
    //printf("exprfree %d\n", exprcount);
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
    // iterate over whole function environment (most is probably empty)
    for(unsigned int ii = 0; ii < ENV_SIZE; ii++) {
        // found a function; has expressions
        if(fenv->env[ii].exprs > 0) {
            func_free(&fenv->env[ii]);
        }
    }
    free(fenv);
}

void
varenv_free(struct varenv* venv)
{
    for(unsigned int ii = 0; ii < ENV_SIZE; ii++) {
        var_free(&venv->env[ii]);
    }
    free(venv);
}  

enum type
expr_to_type(struct expr* e)
{
    switch(e->exprtype) {
        case FUNCTION: return e->e.func.t;
        case LITERAL:  return e->e.lit.t;
        case VARIABLE: return e->e.var.t;
    } panic("can't deduce type from expression!");
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
        // reserved symbols
        case DEFUN: // define the builtin 'defun' function
            //printf("defun\n");
            panic("function definitions only at top level!");
        case DEVAR:
            //printf("devar\n");
            // parsing devar is also done at top level, so add metadata here
            e->e.func.ft = BUILTIN;
            e->e.func.name.b = F_DEVAR;
            e->e.func.exprs = 0;
            e->e.func.argnum = 0;
            parse_devar(fenv, venv);
            break;
        case RET:
            //printf("ret\n");
            parse_ret(e, fenv, venv);
            break;
        case PRINT:
            //printf("print\n");
            parse_print(e, fenv, venv);
            break;
        case QUINARY:
            //printf("quinary\n");
            parse_quinary(e, fenv, venv);
            break;
        case WHILE:
            //printf("while\n");
            parse_while(e, fenv, venv);
            break;

        // assignment
        case INC:
            //printf("inc\n");
            break;
        case DEC:
            //printf("inc\n");
            break;
        case ASSIGN:
            //printf("assign\n");
            parse_assign(e, fenv, venv);
            break;

        // arithmetic
        case ADD: { e->e.func.name.b = F_ADD; goto builtin_binops; }
        case SUB: { e->e.func.name.b = F_SUB; goto builtin_binops; }
        case MUL: { e->e.func.name.b = F_MUL; goto builtin_binops; }
        case DIV: { e->e.func.name.b = F_DIV; goto builtin_binops; }
        case MOD: { e->e.func.name.b = F_MOD; goto builtin_binops; }

        // binary operators
        case NOT: { e->e.func.name.b = F_NOT; panic("uh this is unary"); }
        case AND: { e->e.func.name.b = F_AND; goto builtin_binops; }
        case OR:  { e->e.func.name.b = F_OR;  goto builtin_binops; }
        case LSL: { e->e.func.name.b = F_LSL; goto builtin_binops; }
        case LSR: { e->e.func.name.b = F_LSR; goto builtin_binops; }

        // comparators
        case EQ:  { e->e.func.name.b = F_EQ;  goto builtin_binops; }
        case NE:  { e->e.func.name.b = F_NE;  goto builtin_binops; }
        case GT:  { e->e.func.name.b = F_GT;  goto builtin_binops; }
        case LT:  { e->e.func.name.b = F_LT;  goto builtin_binops; }
        case GE:  { e->e.func.name.b = F_GE;  goto builtin_binops; }
        case LE:  { e->e.func.name.b = F_LE;  goto builtin_binops; }
        builtin_binops:
            //printf("binop\n");
            parse_binop(e, fenv, venv);
            break;

        // symbol preceded by LPAREN must be a function reference
        case SYM:
            //printf("func call\n");
            parse_call(e, fenv, venv);
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
        //printf("int literal\n");
        e->exprtype = LITERAL;
        e->e.lit.t = INT;
        e->e.lit.litval.integer = currtok.value.num;
        break;
    case A_STR:
        //printf("string literal\n");
        e->exprtype = LITERAL;
        e->e.lit.t = STRING;
        memcpy(e->e.lit.litval.string, currtok.value.str, MAX_STRLEN + 1);
        break;

    default:
        fprintf(stderr, "unexpected expression token %d!\n", currtok.type);
        exit(-1);
    }
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
    fenv->env[name].venv = malloc(sizeof(struct varenv));
    memcpy(fenv->env[name].venv, venv, sizeof(struct varenv));

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
        enum type t = typetok_to_type(currtok.type);
        fenv->env[name].args.argt[ii] = t; argvar->t = t;
        fenv->env[name].argnum++;
        checktok((currtok = scan()), RPAREN, "function argument end");
        assert(ii < MAXARGS && "maximum function arguments, increase MAXARGS!");
    }
    //printf("parsed %d for func %ld\n", fenv->env[name].argnum, name);
    checktok(currtok, RPAREN, "function signature end");

    // FUNCTION BODY PARSING
    fenv->env[name].exprs = 0;
    for(currtok = scan(); currtok.type == LPAREN; currtok = scan()) {
        checktok(currtok, LPAREN, "function body expression begin");
        unsigned int ii = fenv->env[name].exprs;
        struct expr* e = (fenv->env[name].body[ii] = expr_init());
        parse_expr(e, fenv, fenv->env[name].venv);
        fenv->env[name].exprs++;
        checktok(currtok, RPAREN, "function body expression end");
        assert(ii < MAXEXPRS && "maximum function expressions, increase MAXEXPRS!");
    }
    assert(fenv->env[name].exprs > 0 && "function must have a body!");
    checktok(currtok, RPAREN, "function declaration end (match defun)");
}

// parse a `devar` declaration
// parsing begins with currtok pointing to '(' after 'devar'
// parsing ends with currtok pointing to ')' after expression body
void
parse_devar(struct funenv* fenv, struct varenv* venv)
{
    //printf("parsing in venv %p\n", (void*)venv);
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

// parse a return expression
// parsing begins with currtok pointing to token after 'ret'
// parsing ends with currtok pointing to ')' matching '(' before 'ret'
void
parse_ret(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    e->e.func.ft = BUILTIN;
    e->e.func.t = VOID;
    e->e.func.name.b = F_RET;
    e->e.func.argnum = 1;
    e->e.func.exprs = 1;
    e->e.func.args.argt[0] = INT;

    currtok = scan();
    parse_expr(e->e.func.body[0] = expr_init(), fenv, venv);
    checktok((currtok = scan()), RPAREN, "return end");
}

// parse a print expression
// parsing begins with currtok pointing to token after 'print'
// parsing ends with currtok pointing to ')' matching '(' before 'print'
void
parse_print(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    e->e.func.t = VOID;
    e->e.func.ft = BUILTIN;
    e->e.func.name.b = F_PRINT;
    e->e.func.argnum = 1;
    e->e.func.exprs = 1;
    e->e.func.args.argt[0] = STRING;

    currtok = scan();
    parse_expr((e->e.func.body[0] = expr_init()), fenv, venv);
    currtok = scan();
}

// parse a quinary expression
// parsing begins with currtok pointing to '(' after '?'
// parsing ends with currtok pointing to ')' matching '(' before '?'
void
parse_quinary(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    // TODO: the consequence branch should be optional!
    e->e.func.ft = BUILTIN;
    e->e.func.name.b = F_QUI;
    e->e.func.exprs = 3;
    e->e.func.argnum = 3;

    struct expr* cond = (e->e.func.body[0] = expr_init());
    struct expr* resl = (e->e.func.body[1] = expr_init());
    struct expr* cons = (e->e.func.body[2] = expr_init());
    checktok((currtok = scan()), LPAREN, "quinary cond beginning");
    parse_expr(cond, fenv, venv);
    currtok = scan(); // first token of result expression
    parse_expr(resl, fenv, venv);
    currtok = scan(); // first token of consequence expression
    parse_expr(cons, fenv, venv);
    checktok((currtok = scan()), RPAREN, "quinary cons end");

    enum type t_resl = expr_to_type(resl);
    enum type t_cons = expr_to_type(cons);
    assert(t_resl  == t_cons &&  "result and consequence must have same type!");
    
    // metadata
    e->e.func.t = t_resl; // quinary return type is type branches
    e->e.func.args.argt[0] = INT;
    e->e.func.args.argt[1] = t_resl;
    e->e.func.args.argt[2] = t_cons;
}

// parse a 'while' expression
// parsing begins with currtok pointing to '(' after 'while' token
// parsing ends with currtok pointint to ')' matching '(' before 'while' token
void
parse_while(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    e->e.func.t = VOID;
    e->e.func.ft = BUILTIN;
    e->e.func.name.b = F_WHILE;
    e->e.func.argnum = 1;
    e->e.func.exprs = 1;

    currtok = scan();
    parse_expr((e->e.func.body[0] = expr_init()), fenv, venv);

    for(currtok = scan(); currtok.type == LPAREN; currtok = scan()) {
        unsigned int ii = e->e.func.exprs;
        parse_expr((e->e.func.body[ii] = expr_init()), fenv, venv);
        e->e.func.exprs++;
        e->e.func.argnum++;
        checktok(currtok, RPAREN, "while body expression end");
    }
    e->e.func.args.argt[0] = INT;
    for(unsigned int ii = 0; ii < e->e.func.argnum; ii++) {
        e->e.func.args.argt[ii] = expr_to_type(e->e.func.body[ii]);
    }
}

// parse an assign expression
// parsing begins with currtok pointing to token after operator
// parsing ends with currtok pointing to ')' matching '(' before operator
void
parse_assign(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    e->e.func.t = VOID;
    e->e.func.ft = BUILTIN;
    e->e.func.name.b = F_ASSGN;
    e->e.func.exprs = 2;
    e->e.func.argnum = 2;

    currtok = scan();
    struct expr* var = (e->e.func.body[0] = expr_init());
    parse_expr(var, fenv, venv);
    assert(var->exprtype == VARIABLE && "assignment must operating on variable!");
    currtok = scan();
    struct expr* new = (e->e.func.body[1] = expr_init());
    parse_expr(new, fenv, venv);
    currtok = scan();
}

// parse a builtin binary expression
// parsing begins with currtok pointing to token after binop
// parsing ends with currtok pointing to ')' matching '(' before binop
void
parse_binop(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    e->e.func.t = INT;
    e->e.func.ft = BUILTIN;
    e->e.func.argnum = 2;
    e->e.func.exprs = 2;
    e->e.func.args.argt[0] = INT;
    e->e.func.args.argt[1] = INT;

    // expressions
    currtok = scan(); // begin first argument
    parse_expr((e->e.func.body[0] = expr_init()), fenv, venv);
    currtok = scan(); // begin second argument
    parse_expr((e->e.func.body[1] = expr_init()), fenv, venv);
    currtok = scan();
}

// parse a function call symbol in the given environments
void
parse_call(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    e->e.func.ft = CALL;
    e->e.func.name.hash = fenv->env[currtok.value.hash].name.hash;
    e->e.func.argnum = fenv->env[currtok.value.hash].argnum;
    e->e.func.exprs = fenv->env[currtok.value.hash].exprs;

    assert(fenv->env[currtok.value.hash].body[0] != NULL
           && "calling an undefined function!");

    currtok = scan();
    for(unsigned int ii = 0; ii < e->e.func.argnum; ii++) {
        assert(currtok.type != RPAREN && "expected another argument!");
        e->e.func.body[ii] = expr_init();
        parse_expr(e->e.func.body[ii], fenv, venv);
        //printf("parsed expr %d\n", ii);
        currtok = scan();
    }
    checktok(currtok, RPAREN, "function arguments end");
}

// parse a top-level expression
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
