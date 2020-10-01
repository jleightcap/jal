#ifndef RISCV64_H
#define RISCV64_H

#include <stdio.h>
#include <stdbool.h>

#include "parse.h"

// ========================================================================= //
//                                                                           //
// RISCV-64 DEFINITIONS                                                      //
//                                                                           //
// ========================================================================= //

enum riscv64_regs {
    X0, RA, SP, GP, TP, T0, T1, T2, S0, S1, A0, A1, A2, A3, A4, A5, A6, A7,
    S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, T3, T4, T5, T6
};

struct riscv64_reg_state {
    // register states indexed with riscv64_regs
    bool reg[32];
};


// ========================================================================= //
//                                                                           //
// RISCV-64 FUNCTIONS                                                        //
//                                                                           //
// ========================================================================= //

void
emit_riscv64(FILE* f, const struct funenv* fenv, const struct varenv* venv);

#endif
