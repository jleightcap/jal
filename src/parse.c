#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "token.h"

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
typetok_to_type(const struct token* tok, const unsigned long int name, struct funenv* fenv)
{
    switch(tok->type) {
        case TYPE_INT:
            fenv->env[name].type = INT; break;
        default:
            fprintf(stderr, "type not recognized!\n"); exit(-1);
    }
}

// parse a function definition.
// tokenizing begins at left parentheses after "defun" token.
void
parse_defun(struct funenv* fenv, struct varenv* venv)
{
    struct token currtok;

    // FUNCTION SIGNATURE PARSING
    currtok = scan(); // (
    parse_checktok(&currtok, LPAREN, "function signature begin");
    currtok = scan(); // function name
    unsigned long name = currtok.value.hash;
    currtok = scan(); // function return type
    typetok_to_type(&currtok, name, fenv);
    // TODO: this is where parsing arguments would occur!
    currtok = scan(); // )
    parse_checktok(&currtok, RPAREN, "function signature end");

    // FUNCTION BODY PARSING
    currtok = scan(); // (
    parse_checktok(&currtok, LPAREN, "function body begin");
    currtok = scan(); // 'return'
    parse_checktok(&currtok, RETRN, "return declaration");
    currtok = scan();
    // TODO: expressions are only numbers now!
    fenv->env[name].body.val = currtok.value.num;
    currtok = scan(); // )
    parse_checktok(&currtok, RPAREN, "function body end");

    currtok = scan();
    parse_checktok(&currtok, RPAREN, "function declaration end (match defun)");
}

void
parse(struct funenv* fenv, struct varenv* venv)
{
    struct token currtok; // not to be confused with Kurtag
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
                break;
                
            default:
                fprintf(stderr, "unexpected token at expression initialization!\n");
                exit(-1);
        }
    }
}
