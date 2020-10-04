#ifndef EVAL_H
#define EVAL_H

#include "parse.h"

// evaluate the expression e of type t in the given environments, 
// returning an expression of a literal type.
// all (defun ...) declarations must be deduced to a literal type at
// compile time.
struct expr
eval(const enum exprtype t, const struct expr* e,
     const struct funenv* fenv, const struct varenv* venv);

void
print_expr(const struct expr* e, int nest);

#endif
