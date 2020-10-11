#ifndef EVAL_H
#define EVAL_H

#include "parse.h"

// evaluate the expression e of type t in the given environments, 
// returning a literal type.
// all (defun ...) declarations must have a body be deduced to a
// literal type at compile time.
struct lit
eval(const enum type t, const struct expr* e,
     const struct funenv* fenv, const struct varenv* venv);

#endif
