#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "token.h"

int
iswhitespace(const char c)
{
    return (c == '\t' || c == '\n' || c == '\v' || c == ' ' || c == EOF);
}

int
issymbol(const char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

int
isnum(const char c)
{
    return (c >= '0' && c <= '9');
}

unsigned long
hashstr(const char* str)
{
    assert(strlen(str) > 0);
    unsigned long hash = str[0];
    for(size_t ii = 0; ii < strlen(str); ii++)
        hash = (hash * 67 + str[ii]) % ENV_SIZE;
    return hash;
}

void
print_type(const enum type t)
{
    switch(t) {
    case INT:    printf("int"); break;
    case STRING: printf("string"); break;
    case VOID:   printf("void"); break;
    }
}

void
print_builtin(const enum builtin b)
{
    switch(b) {
    // arithmetic
    case F_ADD:   printf("'+'");   break;
    case F_SUB:   printf("'-'");   break;
    case F_MUL:   printf("'*'");   break;
    case F_DIV:   printf("'/'");   break;
    case F_MOD:   printf("mod");   break;
    // binary operators
    case F_NOT:   printf("not");   break;
    case F_AND:   printf("and");   break;
    case F_OR:    printf("or");    break;
    case F_LSL:   printf("<<");    break;
    case F_LSR:   printf(">>");    break;
    // comparators
    case F_EQ:    printf("==");    break;
    case F_NE:    printf("!=");    break;
    case F_GT:    printf(">");     break;
    case F_LT:    printf("<");     break;
    case F_GE:    printf(">=");    break;
    case F_LE:    printf("<=");    break;
    // special function tokens
    case F_ASSGN: printf("=");     break;
    case F_INC:   printf("++");    break;
    case F_PEQ:   printf("+=");    break;
    case F_DEC:   printf("--");    break;
    case F_MEQ:   printf("-=");    break;
    case F_QUI:   printf("?");     break;
    case F_MAIN:  printf("main");  break;
    case F_DEFUN: printf("defun"); break;
    case F_DEVAR: printf("devar"); break;
    case F_RET:   printf("ret");   break;
    case F_PRINT: printf("print"); break;
    case F_WHILE: printf("while"); break;
    }
}

// print a nice looking indent
#define indt(x) \
    for(unsigned int ii = 0; ii < x; ii++) \
        printf(" "); \
    printf("\033[1m\033[32m|-\033[m");

void
print_fenv(const struct funenv* f) {
    // iterate over whole function environment, most is probably empty
    for(unsigned int ii = 0; ii < ENV_SIZE; ii++) {
        // found a function; has expressions
        if(f->env[ii].exprs > 0) {
            print_defun(&f->env[ii]);
        }
    }
}

void
print_venv(const struct varenv* v)
{
    panic("TODO: variable definition doesn't allow for knowing usage!");
}

// print a top-level defun, calls print_expr on each body expression
void
print_defun(const struct func* f)
{
    indt(0); // -Wno-type-limits
    switch(f->ft) {
    case BUILTIN: 
        printf("defun "); print_builtin(f->name.b); printf(": ");
        break;
    case DEF:
        if(f->name.hash == hashstr("main"))
            printf("defun main: ");
        else
            printf("defun [%ld]: ", f->name.hash);
        break;
    case CALL:
        panic("function call can't be top level!")
    }
    printf("\n");
    for(unsigned int ii = 0; ii < f->exprs; ii++) {
        print_expr(f->body[ii], 2);
    }
}

// print a function call, calls print_expr on each body expression
void
print_func(const struct func* f, const unsigned int nest) {
    switch(f->ft) {
    case DEF:
        panic("function definition not at top level!");
    case BUILTIN:
        printf("func "); print_builtin(f->name.b); printf(": "); printf("\n");
        // TODO: print function signature
        // print body
        for(unsigned int ii = 0; ii < f->exprs; ii++) {
            print_expr(f->body[ii], nest + 1);
        }
        break;
    case CALL:
        if(f->name.hash == hashstr("main")) { // only mandatory hash
            panic("main is only implicitly called!");
        }
        else {
            printf("func [%ld]: ", f->name.hash); printf("\n");
            // TODO: print function signature
            // print body
            for(unsigned int ii = 0; ii < f->argnum; ii++) {
                print_expr(f->body[ii], nest + 1);
            }
        }
        break;
    }
}

void
print_lit(const struct lit* l, const unsigned int nest)
{
    switch(l->t) {
    case INT:
        printf("%d :integer literal\n", l->litval.integer);
        break;
    case STRING:
        printf("\"%s\" :string literal\n", l->litval.string);
        break;
    case VOID:
        printf("void literal\n");
        break;
    default:
        panic("printing unsuported type literal!");
    }
}

void
print_var(const struct var* v, const unsigned int nest)
{
    printf("[%ld] :variable hash\n", v->hash);
}

// recursively print an expression.
void
print_expr(const struct expr* e, const unsigned int nest)
{
    indt(nest);
    switch(e->exprtype) {
    case FUNCTION:
        //printf("function\n");
        print_func(&e->e.func, nest + 1);
        break;
    case LITERAL:
        //printf("literal\n");
        print_lit(&e->e.lit, nest + 1);
        break;
    case VARIABLE:
        //printf("variable\n");
        print_var(&e->e.var, nest + 1);
        break;
    }
}

// convert a token holding a type to the type associated with a given name
enum type
typetok_to_type(const enum toktype t)
{
    switch(t) {
    case TYPE_INT:
        return INT;
    case TYPE_STR:
        return STRING;
    case TYPE_VOID:
        return VOID;
    default:
        fprintf(stderr, "type %d not recognized!\n", t); exit(-1);
    }
}

