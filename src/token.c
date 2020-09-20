#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "util.h"

static int fp = 0; // file position

struct token
scan(const char* file, const int len)
{
    int numlit;
    struct token tok;

    // skip whitespace and comments ('#' to '\n' or EOF)
    while(iswhitespace(file[fp])) fp++;
    if(file[fp] == '#') {
        while(file[fp] != '\n' && file[fp] != EOF) fp++;
        while(iswhitespace(file[fp])) fp++;
    }
    if(fp >= len) {
        tok.type = END;
        return tok;
    }

    // symbols
    if(issymbol(file[fp])) {
        unsigned long hash = file[fp];
        // symbols can't start with number characters, but can include them (ex uint_64)
        while(issymbol(file[fp]) || isnum(file[fp])) {
            // TODO: play with prime numbers in hashing: larger seem to exceed size of ulong
            hash = (hash * 67 + file[fp]) % ENV_SIZE;
            fp++;
        }

        // reserved keywords
        if(hash == hashstr("main"))
            tok.type = MAIN;
        else if(hash == hashstr("defun"))
            tok.type = DEFUN;
        else if (hash == hashstr("int"))
            tok.type = TYPE_INT;

        // new define
        else {
            tok.type = SYMVAR;
        }

        tok.value.hash = hash;
        return tok;
    }

    // integer literals
    if(isnum(file[fp])) {
        numlit = 0;
        while(isnum(file[fp]))
            numlit = (numlit * 10) + file[fp++] - '0';
        tok.type = INTLIT;
        tok.value.num = numlit;
        return tok;
    }

    if (file[fp] == '(') {
        fp++;
        tok.type = LPAREN;
        return tok;
    }

    if (file[fp] == ')') {
        fp++;
        tok.type = RPAREN;
        return tok;
    }

    else {
        fprintf(stderr, "token parsing error!\n");
        exit(-1);
    }
}
