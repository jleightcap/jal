#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "emit.h"
#include "eval.h"
#include "parse.h"
#include "token.h"
#include "util.h"

static struct token currtok;
static unsigned int exprcount = 0;

struct expr*
expr_init(void)
{
    exprcount++;
    //printf("exprinit %d\n", exprcount);
    return malloc(sizeof(struct expr));
}

// initialize function metadata (everything but body)
// because of tagged unions, seems like the safest way to initialize all func
// cases is just pass every possible parameter? gcc has `transparent_union` but
// want this to be compiler agnostic.
void
func_init(struct func* f, const enum type t, const enum ftype ft,
          const unsigned long hash, const enum builtin b,
          const unsigned int argnum, const unsigned int exprs,
          const struct varenv* parent_venv)
{
    f->t = t;
    f->ft = ft;
    f->argnum = argnum;
    f->exprs = exprs;
    switch(f->ft) {
    case FT_DEF:
        f->name.hash = hash;
        // function definitions inheret the upper variable environment
        f->venv = malloc(sizeof(struct varenv));
        memcpy(f->venv, parent_venv, sizeof(struct varenv));
        break;
    case FT_BUILTIN:
        f->name.b = b;
        break;
    case FT_IMPORT:
    case FT_CALL:
        f->name.hash = hash;
        break;
    }
}

void
func_free(struct func* f)
{
    switch(f->ft) {
    case FT_DEF:
        for(unsigned int ii = 0; ii < f->exprs; ii++) {
            expr_free(f->body[ii]);
        }
        free(f->venv);
        break;
    case FT_BUILTIN:
    case FT_IMPORT:
    case FT_CALL:
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
        currtok = fscan(); // operator
        switch(currtok.type) {
        // reserved symbols
        case DEFUN: // define the builtin 'defun' function
            //printf("defun\n");
            panic("function definition not at top level!");
        case DEVAR:
            //printf("devar\n");
            func_init(&e->e.func, VOID, FT_BUILTIN, 0, F_DEVAR, 0, 0, venv);
            parse_devar(fenv, venv);
            break;
        case RET:
            //printf("ret\n");
            func_init(&e->e.func, VOID, FT_BUILTIN, 0, F_RET, 1, 1, venv);
            parse_ret(e, fenv, venv);
            break;
        case QUINARY:
            //printf("quinary\n");
            // TODO: the consequence branch should be optional!
            func_init(&e->e.func, VOID, FT_BUILTIN, 0, F_QUI, 3, 3, venv);
            parse_quinary(e, fenv, venv);
            break;
        case WHILE:
            //printf("while\n");
            func_init(&e->e.func, VOID, FT_BUILTIN, 0, F_WHILE, 1, 1, venv);
            parse_while(e, fenv, venv);
            break;

        // functions that assign
        case ASSIGN:
            //printf("assign\n");
            func_init(&e->e.func, VOID, FT_BUILTIN, 0, F_ASSGN, 2, 2, venv);
            parse_assign(e, fenv, venv);
            break;
        case INC:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_INC, 1, 1, venv);
            goto builtin_uniops;
        case PEQ:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_PEQ, 2, 2, venv);
            goto builtin_binops;
        case DEC:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_DEC, 1, 1, venv);
            goto builtin_uniops;
        case MEQ:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_MEQ, 2, 2, venv);
            goto builtin_binops;

        // arithmetic
        case ADD:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_ADD, 2, 2, venv);
            goto builtin_binops;
        case SUB:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_SUB, 2, 2, venv);
            goto builtin_binops;
        case MUL:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_MUL, 2, 2, venv);
            goto builtin_binops;
        case DIV:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_DIV, 2, 2, venv);
            goto builtin_binops;
        case MOD:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_MOD, 2, 2, venv);
            goto builtin_binops;

        // binary operators
        case NOT:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_NOT, 1, 1, venv);
            goto builtin_binops;
        case AND:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_AND, 2, 2, venv);
            goto builtin_binops;
        case OR:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_OR,  2, 2, venv);
            goto builtin_binops;
        case LSL:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_LSL, 2, 2, venv);
            goto builtin_binops;
        case LSR:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_LSR, 2, 2, venv);
            goto builtin_binops;

        // comparators
        case EQ:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_EQ, 2, 2, venv);
            goto builtin_binops;
        case NE:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_NE, 2, 2, venv);
            goto builtin_binops;
        case GT:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_GT, 2, 2, venv);
            goto builtin_binops;
        case LT:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_LT, 2, 2, venv);
            goto builtin_binops;
        case GE:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_GE, 2, 2, venv);
            goto builtin_binops;
        case LE:
            func_init(&e->e.func, INT, FT_BUILTIN, 0, F_LE, 2, 2, venv);
            goto builtin_binops;

        // unary operators: (operator [EXPR]1)
        builtin_uniops:
            //printf("uniop\n");
            parse_uniop(e, fenv, venv);
            break;

        // binary operators: (operator [EXPR]1 [EXPR]2)
        builtin_binops:
            //printf("binop\n");
            parse_binop(e, fenv, venv);
            break;

        // symbol preceded by LPAREN must be a function reference
        case SYM: {
            //printf("func call\n");
            struct func* f = &fenv->env[currtok.value.hash];
            switch(f->ft) {
            case FT_DEF: // calling something previously defined
                func_init(&e->e.func, f->t, FT_CALL,
                          f->name.hash, 0, f->argnum, f->exprs, venv);
                break;
            case FT_IMPORT: // calling imported function
                func_init(&e->e.func, f->t, FT_IMPORT,
                          f->name.hash, 0, f->argnum, f->exprs, venv);
                break;
            default:
                fprintf(stderr,
                        "expected callable function, got an enexpected ftype %d!\n",
                        f->ft);
                exit(-1);
            }
            parse_call(e, fenv, venv);
            break;
            }
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
    checktok((currtok = fscan()), LPAREN, "function signature begin");
    currtok = fscan(); // function name
    const unsigned long name = currtok.value.hash;
    currtok = fscan(); // function return type
    fenv->env[name].t = typetok_to_type(currtok.type);
    fenv->env[name].ft = FT_DEF;
    fenv->env[name].name.hash = name;

    // ENVIRONMENT INHERITANCE
    fenv->env[name].venv = malloc(sizeof(struct varenv));
    memcpy(fenv->env[name].venv, venv, sizeof(struct varenv));

    // FUNCTION ARGUMENT PARSING
    fenv->env[name].argnum = 0;
    for(currtok = fscan(); currtok.type == LPAREN; currtok = fscan()) {
        checktok(currtok, LPAREN, "function argument begin");
        unsigned int ii = fenv->env[name].argnum;
        currtok = fscan(); // variable
        unsigned long argname = currtok.value.hash;
        fenv->env[name].args.arghash[ii] = argname; // map each argument to venv hash
        struct var* argvar = &venv->env[argname];
        currtok = fscan(); // type
        enum type t = typetok_to_type(currtok.type);
        fenv->env[name].args.argt[ii] = t; argvar->t = t;
        fenv->env[name].argnum++;
        checktok((currtok = fscan()), RPAREN, "function argument end");
        assert(ii < MAXARGS && "maximum function arguments, increase MAXARGS!");
    }
    //printf("parsed %d for func %ld\n", fenv->env[name].argnum, name);
    checktok(currtok, RPAREN, "function signature end");

    // FUNCTION BODY PARSING
    fenv->env[name].exprs = 0;
    for(currtok = fscan(); currtok.type == LPAREN; currtok = fscan()) {
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
    checktok((currtok = fscan()), LPAREN, "variable signature begin");
    currtok = fscan(); // variable name
    const unsigned long name = currtok.value.hash;
    currtok = fscan(); // variable type
    venv->env[name].t = typetok_to_type(currtok.type);
    checktok((currtok = fscan()), RPAREN, "variable signature end");

    // VARIABLE BODY PARSING
    currtok = fscan();
    struct expr* tmp = expr_init();
    parse_expr(tmp, fenv, venv);
    struct lit ans = eval(venv->env[name].t, tmp, fenv, venv);
    expr_free(tmp);

    venv->env[name].lit = ans;
    checktok((currtok = fscan()), RPAREN, "variable body end");
}

// parse an `import` statement
// parsing begins with currtok pointing to token `import`
// parsing ends with currtok pointing to ')' after statement body
//
// returns the file path to import, which is needed in the resulting
// emitted assembly
void
parse_import(struct funenv* fenv, struct varenv* venv)
{
    int ifd;
    struct stat isb;
    const char* importcontent;

    // fscan token for file path
    checktok((currtok = fscan()), A_STR, "import path");

    // open that file path
    const char* importpath = currtok.value.str;
    if(!(ifd = open(importpath, O_RDONLY))) {
        fprintf(stderr, "parse_import: unable to open %s\n", importpath);
        exit(-1);
    }
    fstat(ifd, &isb); // stat import file for size
    if((importcontent = mmap(NULL, isb.st_size, PROT_READ, MAP_SHARED, ifd, 0)) == MAP_FAILED) {
        fprintf(stderr, "parse_import: failed to mmap %s\n", importpath);
        exit(-1);
    }
    const int ilen = isb.st_size;

    // add that 'import' to the top of the emitted file
    emit_include(importpath);

    // parse imported function signatures
    int ifp = 0;
    unsigned int argnum = 0;
    char* fname = calloc(MAX_STRLEN, sizeof(char));
    struct token importtok;

    scan(&importtok, importcontent, &ifp, ilen);
    checktok(importtok, LPAREN, "import signature begin");
    scan_symbol_name(fname, &importtok, importcontent, &ifp);
    checktok(importtok, SYM, "import function name");
    unsigned long name = importtok.value.hash;
    scan(&importtok, importcontent, &ifp, ilen);
    enum type rettype = typetok_to_type(importtok.type);

    for(scan(&importtok, importcontent, &ifp, ilen);
        importtok.type == LPAREN;
        scan(&importtok, importcontent, &ifp, ilen))
    {
        argnum++;
        scan(&importtok, importcontent, &ifp, ilen); // arg[argnum] name
        fenv->env[name].args.arghash[argnum] = importtok.value.hash;
        scan(&importtok, importcontent, &ifp, ilen); // arg[argnum] type
        fenv->env[name].args.argt[argnum] = typetok_to_type(importtok.type);
        scan(&importtok, importcontent, &ifp, ilen);
        checktok(importtok, RPAREN, "import function argument end");

    }
    func_init(&fenv->env[name], rettype, FT_IMPORT, name, 0, argnum, 0, venv);
    memcpy(fenv->env[name].name.import, fname, MAX_STRLEN);
    //printf("%s hash %ld\n", fenv->env[name].name.import, importtok.value.hash);

    // clean up
    free(fname);
    close(ifd);
    checktok((currtok = fscan()), RPAREN, "import statement end");
}

// parse a return expression
// parsing begins with currtok pointing to token after 'ret'
// parsing ends with currtok pointing to ')' matching '(' before 'ret'
void
parse_ret(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    currtok = fscan();
    parse_expr(e->e.func.body[0] = expr_init(), fenv, venv);
    checktok((currtok = fscan()), RPAREN, "return end");
}

// parse a quinary expression
// parsing begins with currtok pointing to '(' after '?'
// parsing ends with currtok pointing to ')' matching '(' before '?'
void
parse_quinary(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    struct expr* cond = (e->e.func.body[0] = expr_init());
    struct expr* resl = (e->e.func.body[1] = expr_init());
    struct expr* cons = (e->e.func.body[2] = expr_init());
    checktok((currtok = fscan()), LPAREN, "quinary cond beginning");
    parse_expr(cond, fenv, venv);
    currtok = fscan(); // first token of result expression
    parse_expr(resl, fenv, venv);
    currtok = fscan(); // first token of consequence expression
    parse_expr(cons, fenv, venv);
    checktok((currtok = fscan()), RPAREN, "quinary cons end");

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
    currtok = fscan(); // conditional expression
    parse_expr((e->e.func.body[0] = expr_init()), fenv, venv);
    e->e.func.args.argt[0] = INT;

    // while expression loop body
    for(currtok = fscan(); currtok.type == LPAREN; currtok = fscan()) {
        unsigned int ii = e->e.func.exprs;
        parse_expr((e->e.func.body[ii] = expr_init()), fenv, venv);
        e->e.func.exprs++;
        e->e.func.argnum++;
        checktok(currtok, RPAREN, "while body expression end");
    }
    // don't actually check if the while expression body contains expressions!
    // a while loop can have an empty body.
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
    // the variable to be assigned to
    currtok = fscan();
    struct expr* var = (e->e.func.body[0] = expr_init());
    parse_expr(var, fenv, venv);
    assert(var->exprtype == VARIABLE && "assignment must operating on variable!");
    // the expression to be assigned
    currtok = fscan();
    struct expr* new = (e->e.func.body[1] = expr_init());
    parse_expr(new, fenv, venv);
    currtok = fscan();
}

// parse a builtin unary expression
// parsing begins with currtok pointing to token after uniop
// parsing ends with currtok pointing to ')' matching '(' before uniop
void
parse_uniop(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    // the one expression in the function body
    currtok = fscan();
    parse_expr((e->e.func.body[0] = expr_init()), fenv, venv);
    currtok = fscan();
}

// parse a builtin binary expression
// parsing begins with currtok pointing to token after binop
// parsing ends with currtok pointing to ')' matching '(' before binop
void
parse_binop(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    // the two expression in the function body
    currtok = fscan(); // begin first argument
    parse_expr((e->e.func.body[0] = expr_init()), fenv, venv);
    currtok = fscan(); // begin second argument
    parse_expr((e->e.func.body[1] = expr_init()), fenv, venv);
    currtok = fscan();
}

// parse a function call symbol in the given environments
void
parse_call(struct expr* e, struct funenv* fenv, struct varenv* venv)
{
    assert((fenv->env[currtok.value.hash].body[0] != NULL ||
            fenv->env[currtok.value.hash].ft == FT_IMPORT)
           && "calling an undefined function!");

    currtok = fscan();
    for(unsigned int ii = 0; ii < e->e.func.argnum; ii++) {
        assert(currtok.type != RPAREN && "expected another argument!");
        e->e.func.body[ii] = expr_init();
        parse_expr(e->e.func.body[ii], fenv, venv);
        //printf("parsed expr %d\n", ii);
        currtok = fscan();
    }
    checktok(currtok, RPAREN, "function arguments end");
}

// parse a top-level expression
void
parse(struct funenv* fenv, struct varenv* venv)
{
    for(currtok = fscan(); currtok.type != END; currtok = fscan()) {
        checktok(currtok, LPAREN, "beginning top level expression");
        currtok = fscan();

        // global definitions, can be:
        // - "defun"
        // - "defvar"
        // - "import"
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
        case IMPORT:
            //printf("import\n");
            parse_import(fenv, venv);
            checktok(currtok, RPAREN, "top level IMPORT end");
            break;
        default:
            fprintf(stderr, "unexpected token %d at top level expression!\n", currtok.type);
            exit(-1);
        }
    }
}
