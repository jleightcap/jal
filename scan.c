#include <stdio.h>
#include <stdlib.h>

#include "scan.h"
#include "slist.h"
#include "util.h"

void
slist_push_back_substr(struct slist* sl, const char* src, const int offset, const int len)
{
    char* buf = substr(src, offset, len);
    slist_push_back(sl, buf, len + 1); // include trailing '\0'
    free(buf);
}

void
scan(const char file[], const int len, struct slist* tokens)
{
    int ii;
    for(ii = 0; ii < len; ii++) {
        const char curr = file[ii];
        if(iswhitespace(curr)) continue;
        slist_push_back_substr(tokens, file, ii, 1);
    }
}
