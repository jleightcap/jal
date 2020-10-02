#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    switch(e->type) {
    case UNARY:
        expr_free(e->body.unary.arg);
        free(e);
        break;
    case BINARY:
        expr_free(e->body.binary.arg1);
        expr_free(e->body.binary.arg2);
        free(e);
        break;
    case LITERAL_INT:
        break;
    }
}

// parse a expression.
// tokenizing begins after RETN token.
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
        case SUB:
        case MUL:
        case DIV:
            printf("intrinsic binary op\n");
            // doesn't this inner switch seem stupid and redundant? probably.
            // want a common body - this seems most easily understandable
            switch(currtok.type) {
            case ADD: e->body.binary.op = PLUS;   break;
            case SUB: e->body.binary.op = MINUS;  break;
            case MUL: e->body.binary.op = TIMES;  break;
            case DIV: e->body.binary.op = DIVIDE; break;
            default: fprintf(stderr, "Unsupported binary op!\n"); exit(-1);
            }
            e->type = BINARY;

            // initialize arg1, associated with same parse arguments
            e->body.binary.arg1 = expr_init();
            currtok = scan();
            parse_expr(name, e->body.binary.arg1, fenv, venv);
            // initialize arg2, associated with same parse arguments
            e->body.binary.arg2 = expr_init();
            currtok = scan();
            parse_expr(name, e->body.binary.arg2, fenv, venv);

            currtok = scan();
            checktok(currtok, RPAREN, "intrinsic binary ops: too many arguments?");
            return;
        default:
            fprintf(stderr, "unexpected operator!\n"); exit(-1);
        }
        return;

    // ATOMS
    case NUM:
        printf("number literal: %d\n", currtok.value.num);
        e->type = LITERAL_INT;
        e->body.val = currtok.value.num;
        return;

    default:
        fprintf(stderr, "unexpected expression!\n"); exit(-1);
    }

    checktok(currtok, RPAREN, "expression end");
}

void
parse_devar(struct varenv* venv)
{
    // VARIABLE SIGNATURE PARSING
    currtok = scan(); // (
    checktok(currtok, LPAREN, "variable signature begin");
    currtok = scan(); // variable name
    const unsigned long name = currtok.value.hash;
    currtok = scan(); // variable type
    venv->env[name].type = INT; // TODO: associate name and type in venv
    currtok = scan(); // )
    checktok(currtok, RPAREN, "variable signature end");

    // VARIABLE BODY PARSING
    currtok = scan(); // TODO: variable bodies are just numbers!
    venv->env[name].value.integer = currtok.value.num;

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
    currtok = scan(); // (
    checktok(currtok, LPAREN, "function body begin");
    currtok = scan(); // 'return'
    checktok(currtok, RETRN, "return declaration");
    fenv->env[name].body = expr_init();
    struct expr* e = fenv->env[name].body;
    currtok = scan(); // token after 'ret'
    parse_expr(name, e, fenv, venv);
    currtok = scan(); // )
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
                parse_defun(fenv, venv);
                break;
            // variable definition
            case DEVAR:
                parse_devar(venv);
                break;
            default:
                fprintf(stderr, "unexpected token at expression initialization!\n");
                exit(-1);
        }
    }
    // TODO: free any occupied fenv expressions
}
