#ifndef _Z80_H
#define _Z80_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "parse.h"

// =========================================================================  //
//                                                                            //
// Z80 DEFINITIONS                                                            //
//                                                                            //
// =========================================================================  //

struct _z80_reg {
    uint8_t  A;
    uint8_t  F;
    uint8_t  B;
    uint8_t  C;
    uint8_t  D;
    uint8_t  E;
    uint16_t IX;
    uint16_t IY;
    uint16_t SP;
};


// =========================================================================  //
//                                                                            //
// Z80 FUNCTIONS                                                              //
//                                                                            //
// =========================================================================  //

void
emit_z80(FILE* f, struct funenv const* fenv, struct varenv const* venv);

#endif
