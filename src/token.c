#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "util.h"

static int fp = 0;              // file position
static const char* file = NULL; // global FILE for tokenizing
static int len = 0;             // global FILE length

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
        // to reduce the number of hash table collisions, keep track of the
        // length of symbol tokens. If some variable name happens to hash
        // to same hash as 'while', lengths must also match for a collision.
        unsigned int symlen = 0;
        unsigned long hash = file[fp];
        // symbols can't start with number characters, but can include them
        for(; issymbol(file[fp]) || isnum(file[fp]); fp++) {
            // TODO: play with prime numbers in hashing
            hash = (hash * 67 + file[fp]) % ENV_SIZE;
            symlen++;
        }
        tok.value.hash = hash;

        // RESERVED KEYWORD HASHES
        if(hash == hashstr("main") && symlen == 4) {
            tok.type = MAIN;
        }
        else if(hash == hashstr("defun") && symlen == 5) {
            tok.type = DEFUN;
        }
        else if(hash == hashstr("devar") && symlen == 5) {
            tok.type = DEVAR;
        }
        else if(hash == hashstr("ret") && symlen == 3) {
            tok.type = RET;
        }
        else if(hash == hashstr("while") && symlen == 5) {
            tok.type = WHILE;
        }
        else if(hash == hashstr("for") && symlen == 3) {
            tok.type = FOR;
        }
        else {
            tok.type = SYM;
        }
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
    // integer literal
    if(isnum(file[fp])) {
        int numlit = 0;
        while(isnum(file[fp]))
            numlit = (numlit * 10) + file[fp++] - '0';
        tok.type = A_INT;
        tok.value.num = numlit;
        return tok;
    }
    // string literal
    if(file[fp] == '"') {
        unsigned int strpos = 0;
        fp++;
        while(file[fp] != '"') {
            switch(file[fp]) {
            case '\\':
                switch(file[++fp]) {
                case 'n':  tok.value.str[strpos] = '\n'; break;
                case 't':  tok.value.str[strpos] = '\t'; break;
                case '\\': tok.value.str[strpos] = '\\'; break;
                default: fprintf(stderr, "unexpected escape sequence!\n"); exit(-1);
                }
                break;
            default: tok.value.str[strpos] = file[fp]; break;
            }
            assert(strpos < MAX_STRLEN && "exceeded string buffer: increase MAX_STRLEN!");
            strpos++; fp++;
        }
        fp++;
        tok.value.str[strpos] = '\0';
        tok.type = A_STR;
        return tok;
    }

    // NON-OVERLAPPING SINGLE CHARACTER TOKENS
    switch(file[fp]) {
    case '(': fp++; tok.type = LPAREN;  return tok;
    case ')': fp++; tok.type = RPAREN;  return tok;
    case '*': fp++; tok.type = MUL;     return tok;
    case '/': fp++; tok.type = DIV;     return tok;
    case '?': fp++; tok.type = QUINARY; return tok;
    default: break;
    }

    // OVERLAPING CHARACTER TOKENS
    switch(file[fp]) {
    case '<': switch(file[fp + 1]) {
              case '<': tok.type = LSL; fp++; break;
              case '=': tok.type = LE;  fp++; break;
              default:  tok.type = LT;        break;
              } fp++; return tok;
    case '>': switch(file[fp + 1]) {
              case '>': tok.type = LSR; fp++; break;
              case '=': tok.type = GE;  fp++; break;
              default:  tok.type = GT;        break;
              } fp++; return tok;
    case '=': switch(file[fp + 1]) {
              case '=': tok.type = EQ;     fp++; break;
              default:  tok.type = ASSIGN;       break;
              } fp++; return tok;
    case '!': switch(file[fp + 1]) {
              case '=': tok.type = NE;  fp++; break;
              default:  tok.type = NOT;       break;
              } fp++; return tok;
    case '+': switch(file[fp + 1]) {
              case '+': tok.type = INC; fp++; break;
              case '=': tok.type = PEQ; fp++; break;
              default:  tok.type = ADD;       break;
              } fp++; return tok;
    case '-': switch(file[fp + 1]) {
              case '-': tok.type = DEC; fp++; break;
              case '=': tok.type = MEQ; fp++; break;
              default:  tok.type = SUB;       break;
              } fp++; return tok;
    default: break;
    }

    // No matches! Tokenizing failed.
    fprintf(stderr, "token parsing error!\n");
    exit(-1);
}
