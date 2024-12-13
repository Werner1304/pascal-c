#pragma once

#include "list.h"
#include "mystring.h"
#include "parsed_ast.h"

enum symbol_type {
    SymbolVariable,
    SymbolConstant,
    SymbolProcedure,
};

struct symbol {
    enum symbol_type kind;
    struct string_view name;
    union {
        int number;
        struct parsed_block *procedure;
    } value;
};

struct symbol symbol_new_variable(struct string_view name);
struct symbol symbol_new_constant(struct string_view name, int value);
struct symbol symbol_new_procedure(struct string_view name,
                                   struct parsed_block *procedure);

DEFINE_LIST(list_symbol, struct symbol);

struct scope {
    struct list_symbol symbols;
};

struct scope scope_new();
struct symbol *scope_find_symbol(struct scope *self, struct string_view name);
void scope_free(struct scope self);

DEFINE_LIST(list_scope, struct scope);

struct interpreter {
    struct list_scope scopes;
};

struct interpreter interpreter_new();
void interpreter_eval_program(struct interpreter *self,
                              struct parsed_program *program);
