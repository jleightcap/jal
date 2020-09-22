#ifndef EVAL_H
#define EVAL_H

#include "token.h"

void
eval_entry(const struct funenv* fenv, const struct varenv* venv);

#endif
