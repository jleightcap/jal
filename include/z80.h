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

struct _z80_reg_state {
    bool A;
    bool F;
    bool B;
    bool C;
    bool D;
    bool E;
    bool IX;
    bool IY;
    bool SP;
};


// =========================================================================  //
//                                                                            //
// Z80 FUNCTIONS                                                              //
//                                                                            //
// =========================================================================  //

void
z80_emit_init(FILE* f);

void
z80_emit_free(void);

void
z80_emit(const struct funenv* fenv, const struct varenv* venv);

void
z80_emit_include(const char* path);

#endif
