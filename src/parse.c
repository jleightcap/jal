#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "token.h"

struct token currtok;

// if 'tok' is not of type 'type', print 'locationstr' error message and exit
void
parse_checktok(const struct token* tok, const enum toktype type, const char* locationstr)
{
    if(tok->type != type) {
        fprintf(stderr, "%s - didn't find expected token!\n", locationstr);
        exit(-1);
    }
}

// convert a token holding a type to the type associated with a given name
void
typetok_to_type(const enum toktype t, const unsigned long int name, struct funenv* fenv)
{
    switch(t) {
        case TYPE_INT:
            fenv->env[name].type = INT; break;
        default:
            fprintf(stderr, "type not recognized!\n"); exit(-1);
    }
}

// parse a expression.
// tokenizing begins after RETN token.
void
parse_expr(const unsigned long name, struct funenv* fenv, struct varenv* venv)
{
    struct expr e;
    currtok = scan();
    switch(currtok.type) {
        // EXPRESSION
        case LPAREN:
            currtok = scan(); // operator
            switch(currtok.type) {
                default:
                    fprintf(stderr, "unexpected operator!\n"); exit(-1);
            }
            return;

        // ATOMS
        case NUM_LIT:
            e.val = currtok.value.num;
            printf("%d\n", e.val);
            return;

        default:
            fprintf(stderr, "unexpected expression!\n"); exit(-1);
    }

    currtok = scan(); // operator
    enum toktype op = currtok.type;
    currtok = scan(); // num1
    int num1 = currtok.value.num;
    currtok = scan(); // num2
    int num2 = currtok.value.num;
    int result;
    switch(op) {
        case ADD:
            result = num1 + num2; break;
        case SUB:
            result = num1 - num2; break;
        case MUL:
            result = num1 * num2; break;
        case DIV:
            result = num1 / num2; break;
        case MOD:
            result = num1 % num2; break;
        default:
            fprintf(stderr, "unrecognized operator!"); exit(-1);
    }
    fenv->env[name].body.val = result;

    currtok = scan(); // 'return'
    parse_checktok(&currtok, RPAREN, "expression end");
}

void
parse_devar(struct funenv* fenv, struct varenv* venv)
{
    // VARIABLE SIGNATURE PARSING
    currtok = scan(); // (
    parse_checktok(&currtok, LPAREN, "variable signature begin");
    currtok = scan(); // variable name
    const unsigned long name = currtok.value.hash;
    currtok = scan(); // variable type
    venv->env[name].type = INT; // TODO: associate name and type in venv
    currtok = scan(); // )
    parse_checktok(&currtok, RPAREN, "variable signature end");

    // VARIABLE BODY PARSING
    currtok = scan(); // TODO: variable bodies are just numbers!
    venv->env[name].value.integer = currtok.value.num;

    currtok = scan();
    parse_checktok(&currtok, RPAREN, "variable defition end (match devar)");
}

// parse a function definition.
// tokenizing begins at left parentheses after "defun" token.
void
parse_defun(struct funenv* fenv, struct varenv* venv)
{
    // FUNCTION SIGNATURE PARSING
    currtok = scan(); // (
    parse_checktok(&currtok, LPAREN, "function signature begin");
    currtok = scan(); // function name
    const unsigned long name = currtok.value.hash;
    currtok = scan(); // function return type
    typetok_to_type(currtok.type, name, fenv);
    // TODO: this is where parsing arguments would occur!
    currtok = scan(); // )
    parse_checktok(&currtok, RPAREN, "function signature end");

    // FUNCTION BODY PARSING
    currtok = scan(); // (
    parse_checktok(&currtok, LPAREN, "function body begin");
    currtok = scan(); // 'return'
    parse_checktok(&currtok, RETRN, "return declaration");
    parse_expr(name, fenv, venv);
    currtok = scan(); // )
    parse_checktok(&currtok, RPAREN, "function body end");

    currtok = scan();
    parse_checktok(&currtok, RPAREN, "function declaration end (match defun)");
}

void
parse(struct funenv* fenv, struct varenv* venv)
{
    for(currtok = scan(); currtok.type != END; currtok=scan()) {
        parse_checktok(&currtok, LPAREN, "beginning expression");
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
                parse_devar(fenv, venv);
                break;
            default:
                fprintf(stderr, "unexpected token at expression initialization!\n");
                exit(-1);
        }
    }
}
