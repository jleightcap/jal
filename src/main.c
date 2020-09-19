#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "scan.h"

int
main(int ac, char** av) {
    const char* file;
    int fd;
    struct stat sb;

    if (ac != 2) {
        fprintf(stderr, "usage: %s infile\n", av[0]);
        return -1;
    }
    if(!(fd = open(av[1], O_RDONLY))) {
        fprintf(stderr, "%s: unable to open %s\n", av[0], av[1]);
        return -1;
    }
    fstat(fd, &sb); // stat input file for size
    if ((file = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        fprintf(stderr, "%s: failed to mmap %s\n", av[0], av[1]);
        return -1;
    }

    char* curr;
    while((curr = scan(file, sb.st_size)) != NULL) {
    }
}
