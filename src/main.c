#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "emit.h"
#include "eval.h"
#include "parse.h"
#include "util.h"

int
main(int ac, char** av) {
    const char* file;
    FILE* outf;
    int fd;
    struct stat sb;

    if (ac != 3) {
        fprintf(stderr, "usage: %s infile outfile\n", av[0]);
        return -1;
    }
    if(!(fd = open(av[1], O_RDONLY))) {
        fprintf(stderr, "%s: unable to open %s\n", av[0], av[1]);
        return -1;
    }
    fstat(fd, &sb); // stat input file for size
    if((file = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        fprintf(stderr, "%s: failed to mmap %s\n", av[0], av[1]);
        return -1;
    }
    if((outf = fopen(av[2], "w")) == NULL) {
        fprintf(stderr, "%s: failed open %s\n", av[0], av[2]);
        return -1;
    }

    struct funenv fenv;        // function environment
    struct varenv venv; // global variable environment
    // zero out environments
    memset(fenv.env, 0, sizeof(fenv.env));
    memset(venv.env, 0, sizeof(venv.env));
    setstream(file, sb.st_size);

    parse(&fenv, &venv);

    unsigned long main = hashstr("main");
    struct expr* mainexpr = fenv.env[main].body;
    print_expr(mainexpr, 0);
    /*
    struct expr ans = eval(LITERAL_INT, mainexp, &fenv, &venv);
    printf("ans = %d\n", ans.body.val);
    */

    emit(outf, &fenv, &venv);


    funenv_free(&fenv);
    varenv_free(&venv);

}
