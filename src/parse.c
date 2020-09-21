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

// parse a function definition.
// tokenizing begins at left parentheses after "defun" token.
void
parse_defun(struct funenv* fenv, struct varenv* venv)
{
    struct token currtok;
    currtok = scan();
    parse_checktok(&currtok, LPAREN, "function name");

    // FUNCTION SIGNATURE PARSING
    currtok = scan(); // function name
    unsigned long name = currtok.value.hash;
    currtok = scan(); // function return type
    switch(currtok.type) {
        case TYPE_INT:
            fenv->env[name].type = INT;
            break;
        default:
            fprintf(stderr, "not recognized type definition!\n"); exit(-1);
    }
    // TODO:
    // this is where parsing arugments would occur!
    
    currtok = scan();
    parse_checktok(&currtok, RPAREN, "function signature");

    // FUNCTION BODY PARSING
    currtok = scan();
    fenv->env[name].body.val = currtok.value.num; // TODO: expressions are only numbers now!

    currtok = scan();
    parse_checktok(&currtok, RPAREN, "function body end");
}

void
parse(struct funenv* fenv, struct varenv* venv)
{
    struct token currtok; // not to be consused with Kurtag
    currtok = scan();
    parse_checktok(&currtok, LPAREN, "beginning expression");
    currtok = scan();

    // top-level definitions, can be:
    // - "defun"
    // - "defvar"
    // - 'main"
    switch(currtok.type) {
        // function definition
        case DEFUN:
            printf("defining function...\n");
            parse_defun(fenv, venv);
            break;

        // variable definition
        case DEVAR:
            printf("defining variable...\n");
            break;
            
        // a symbol: must be call to defined function
        case SYM:
        case SYMFUN:
            printf("function call\n");
            break;

        default:
            fprintf(stderr, "unexpected token at expression initialization!\n");
            exit(-1);
    }
}
