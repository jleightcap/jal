#ifndef _6502_H
#define _6502_H

#include <stdio.h>
#include <stdbool.h>

#include "parse.h"

// =========================================================================  //
//                                                                            //
// 6502 DEFINITIONS                                                           //
//                                                                            //
// =========================================================================  //

enum _6502_regs {
    A, X, Y
};

struct _6502_reg_state {
    // register states indexed with riscv64_regs
    bool reg[3];
};


// =========================================================================  //
//                                                                            //
// 6502 FUNCTIONS                                                             //
//                                                                            //
// =========================================================================  //

void
emit_6502(FILE* f, struct funenv const* fenv, struct varenv const* venv);

#endif
