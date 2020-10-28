#ifndef _Z80_H
#define _Z80_H

#include <stdio.h>
#include <stdbool.h>

#include "parse.h"

// =========================================================================  //
//                                                                            //
// 6502 DEFINITIONS                                                           //
//                                                                            //
// =========================================================================  //

enum _z80_regs {
    A, X, Y
};

struct _z80_reg_state {
    // register states indexed with riscv64_regs
    bool reg[3];
};


// =========================================================================  //
//                                                                            //
// 6502 FUNCTIONS                                                             //
//                                                                            //
// =========================================================================  //

void
emit_z80(FILE* f, struct funenv const* fenv, struct varenv const* venv);

#endif
