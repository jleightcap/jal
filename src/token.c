#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "util.h"

static int fp = 0; // file position
const char* file = NULL;
int len = 0;

void
setstream(const char stream[], int streamlen)
{
    file = stream;
    len = streamlen;
}

struct token
scan()
{
    assert(file != NULL && "file stream not initialized: use setstream!");
    assert(len  != 0    && "len not initialized: use setstream!");

    int numlit;
    struct token tok;

    // skip whitespace and comments ('#' to '\n' or EOF)
    while(iswhitespace(file[fp])) fp++;
    while(file[fp] == '#') {
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
        else if(hash == hashstr("devar"))
            tok.type = DEVAR;
        else if (hash == hashstr("int"))
            tok.type = TYPE_INT;

        // new symbol
        else {
            tok.type = SYM;
        }

        tok.value.hash = hash;
        return tok;
    }
    // integer literals
    if(isnum(file[fp])) {
        numlit = 0;
        while(isnum(file[fp]))
            numlit = (numlit * 10) + file[fp++] - '0';
        tok.type = NUM_LIT;
        tok.value.num = numlit;
        return tok;
    }
    // lparen
    if(file[fp] == '(') {
        fp++;
        tok.type = LPAREN;
        return tok;
    }
    // rparen
    if(file[fp] == ')') {
        fp++;
        tok.type = RPAREN;
        return tok;
    }
    // +
    if(file[fp] == '+') {
        fp++;
        tok.type = ADD;
        return tok;
    }
    // -
    if(file[fp] == '-') {
        fp++;
        tok.type = SUB;
        return tok;
    }
    // *
    if(file[fp] == '*') {
        fp++;
        tok.type = MUL;
        return tok;
    }
    // /
    if(file[fp] == '/') {
        fp++;
        tok.type = DIV;
        return tok;
    }

    else {
        fprintf(stderr, "token parsing error!\n");
        exit(-1);
    }
}
