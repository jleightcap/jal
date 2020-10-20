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

    struct funenv* fenv = calloc(1, sizeof(struct funenv)); // function environment
    struct varenv* venv = calloc(1, sizeof(struct varenv)); // global variable environment
    setstream(file, sb.st_size);

    parse(fenv, venv);

    // emit assembly from 'main' expression tree entrypoint
    emit(outf, fenv, venv);

    funenv_free(fenv);
    varenv_free(venv);

    return 0;
}
