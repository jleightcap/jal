#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "util.h"

static int fp = 0;              // file position
static const char* file = NULL; // global FILE for tokenizing
int len = 0;                    // global FILE length

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

    // SYMBOLS
    if(issymbol(file[fp])) {
        unsigned long hash = file[fp];
        // symbols can't start with number characters, but can include them
        for(; issymbol(file[fp]) || isnum(file[fp]); fp++) {
            // TODO: play with prime numbers in hashing
            hash = (hash * 67 + file[fp]) % ENV_SIZE;
        }
        tok.value.hash = hash;
        // RESERVED KEYWORD HASHES
        if(hash == hashstr("main"))
            tok.type = MAIN;
        else
        if(hash == hashstr("defun"))
            tok.type = DEFUN;
        else
        if(hash == hashstr("devar"))
            tok.type = DEVAR;
        else
        if(hash == hashstr("ret"))
            tok.type = RET;
        else
        if(hash == hashstr("print"))
            tok.type = PRINT;
        else
            tok.type = SYM;
        return tok;
    }

    // TYPES
    if(file[fp] == '`') {
        fp++;
        unsigned long hash = file[fp];
        for(; issymbol(file[fp]); fp++) {
            hash = (hash * 67 + file[fp]) % ENV_SIZE;
        }
        tok.value.hash = hash;
        
        // RESERVED TYPE HASHES
        if(hash == hashstr("int"))
            tok.type = TYPE_INT;
        else
        if(hash == hashstr("string"))
            tok.type = TYPE_STR;
        else
        if(hash == hashstr("void"))
            tok.type = TYPE_VOID;
        else
            fprintf(stderr, "invalid type declaration!\n");

        return tok;
    }

    // LITERALS
    // integer
    if(isnum(file[fp])) {
        int numlit = 0;
        while(isnum(file[fp]))
            numlit = (numlit * 10) + file[fp++] - '0';
        tok.type = A_INT;
        tok.value.num = numlit;
        return tok;
    }
    // string
    if(file[fp] == '"') {
        unsigned int strpos = 0;
        fp++;
        while(file[fp] != '"') {
            switch(file[fp]) {
            // escape characters
            case '\\':
                switch(file[++fp]) {
                case 'n':
                    tok.value.str[strpos] = '\n';
                    break;
                default:
                    fprintf(stderr, "unexpected escape sequence!\n"); exit(-1);
                }
                break;
            // non-escape characters
            default:
                tok.value.str[strpos] = file[fp];
                break;
            }
            assert(strpos < MAX_STRLEN && "exceeded string buffer: increase MAX_STRLEN!");
            strpos++; fp++;
        }
        fp++;
        tok.value.str[strpos] = '\0';
        tok.type = A_STR;
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
