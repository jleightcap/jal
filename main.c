#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "slist.h"
#include "scan.h"

int
main(int ac, char** av)
{
    const char* file;
    int fd;
    struct stat sb;

    if (ac != 2) {
        fprintf(stderr, "usage: %s infile\n", av[0]);
        return -1;
    }

    fd = open(av[1], O_RDONLY);
    if (fd == 0) {
        fprintf(stderr, "%s: unable to open %s\n", av[0], av[1]);
        return -1;
    }
    fstat(fd, &sb);
    file = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (file == MAP_FAILED) {
        fprintf(stderr, "%s: failed to mmap %s\n", av[0], av[1]);
        return -1;
    }

    scan(&file, sb.st_size);
}
