#include <stdio.h>
#include "slist.h"

int
main(int ac, char** av)
{
    FILE* f;

    if (ac != 2) {
        fprintf(stderr, "usage: %s infile\n", av[0]);
        return -1;
    }

    f = fopen(av[1], "r");
    if (f == NULL) {
        fprintf(stderr, "%s: unable to open %s\n", av[0], av[1]);
        return -1;
    }

    struct slist* tokens = slist_init();
    slist_push_back(tokens, "hello");
    slist_push_back(tokens, "world");
    slist_print(tokens);
}
