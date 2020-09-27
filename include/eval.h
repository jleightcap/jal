#ifndef EVAL_H
#define EVAL_H

#include "parse.h"

void
eval_entry(const struct funenv* fenv, const struct varenv* venv);

#endif
