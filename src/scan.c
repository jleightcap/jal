#include <stdio.h>
#include <stdlib.h>

#include "scan.h"
#include "util.h"

static int fp = 0; // file position

char*
scan(char* file, const int len)
{
    int startfp;
    long numlit;
    unsigned long hash;

    // skip whitespace and comments ('#' to '\n' or EOF)
    while(iswhitespace(file[fp])) fp++;
    if(file[fp] == '#') {
        while(file[fp] != '\n' && file[fp] != EOF) fp++;
        while(iswhitespace(file[fp])) fp++;
    }
    if(fp >= len) return NULL; // skipped to end of file, return

    // symbols
    if(issymbol(file[fp])) {
        hash = file[fp];
        startfp = fp;
        // symbols can't start with number characters, but can include them (ex uint_64)
        while(issymbol(file[fp]) || isnum(file[fp])) {
            printf("%c ", file[fp]);
            hash = hash * 67 + file[fp]; // "not terrible" - Nat Tuck 10/1/2019
            fp++;
        }
        printf("\t\thash = %ld\n", hash);
        return "a";
    }

    // integer literals
    if(isnum(file[fp])) {
        numlit = 0;
        startfp = fp;
        // scan to end of number literal for length
        while(isnum(file[fp])) fp++;
        char* last = file + fp;
        numlit = strtol(file + startfp, &last, 10); // see scan.h TODO on file const
        printf("%ld\n", numlit);
        return "a";
    }

    printf("%c\n", file[fp]);
    fp++;
    return "a";
}
