#include <stdio.h>
#include <stdlib.h>

#include "scan.h"
#include "util.h"

static int fp = 0; // file position

char*
scan(const char* file, const int len)
{
    char curr;
    int hash, startfp;

    // skip whitespace and comments (# to \n or EOF)
    while(iswhitespace(file[fp])) fp++;
    if(curr == '#') {
        // comments
        while(file[fp] != '\n' && file[fp] != EOF) fp++;
        fp++; // move past \n or EOF
        while(iswhitespace(file[fp])) fp++;
    }

    curr = file[fp];
    if(issymbol(curr)) {
        hash = curr;
        startfp = fp;
        while(issymbol(curr) || (curr >= '0' && curr <= '9')) {
            printf("%c ", file[fp]);
            hash = hash * 147 + curr;
            curr = file[fp++];
        }
        printf("\nlen=%d\n", fp - startfp);
        //printf("hash = %d\n", hash);
    }

    if(fp >= len) return NULL;
    else {
        printf("not symbol: %c\n", file[fp]);
        fp++;
        return "a";
    }
}
