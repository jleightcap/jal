#ifndef SCAN_H
#define SCAN_H

#include "slist.h"

// TODO: make file const: atol's char** endptr doesn't allow const
char*
scan(char* file, const int len);

#endif
