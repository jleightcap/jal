fenv ::= funv[]
venv ::= var[]

expr ::= {func, lit, var}

func ::= {fenv@, builtin}
         type
         {expr[], 0}
         venv

lit ::= type
        int_lit
        string_lit

var ::= venv@
        type
        expr
