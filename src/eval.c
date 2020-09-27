#include <stdio.h>

#include "eval.h"
#include "parse.h"
#include "token.h"
#include "util.h"

void
eval(const struct expr* e)
{
    printf("eval: %d\n", e->val);
}

void
eval_entry(const struct funenv* fenv, const struct varenv* venv)
{
    // entrypoint is "main"
    unsigned long main = hashstr("main");
    struct expr mainfunc = fenv->env[main].body;
    eval(&mainfunc);
}

