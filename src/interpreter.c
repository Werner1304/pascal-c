#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

IMPL_LIST(list_symbol, struct symbol);
IMPL_LIST(list_scope, struct scope);

struct symbol symbol_new_variable(struct string_view name) {
    struct symbol s = {.kind = SymbolVariable, .name = name, .value.number = 0};
    return s;
}

struct symbol symbol_new_constant(struct string_view name, int value) {
    struct symbol s = {
        .kind = SymbolConstant, .name = name, .value.number = value};
    return s;
}

struct symbol symbol_new_procedure(struct string_view name,
                                   struct parsed_block *procedure) {
    struct symbol s = {
        .kind = SymbolProcedure, .name = name, .value.procedure = procedure};
    return s;
}

struct scope scope_new() {
    struct scope s = {
        .symbols = list_symbol_new(4),
    };
    return s;
}

struct symbol *scope_find_symbol(struct scope *self, struct string_view name) {
    for (int i = 0; i < self->symbols.length; i++) {
        if (string_view_compare(self->symbols.buffer[i].name, name)) {
            return &self->symbols.buffer[i];
        }
    }

    return NULL;
}

void scope_free(struct scope self) { list_symbol_free(self.symbols); }

struct interpreter interpreter_new() {
    struct interpreter i = {.scopes = list_scope_new(2)};
    return i;
}

void inter_push_scope(struct interpreter *self) {
    list_scope_push(&self->scopes, scope_new());
}

void inter_pop_scope(struct interpreter *self) {
    list_scope_pop(&self->scopes);
}

struct scope *inter_global_scope(struct interpreter *self) {
    if (self->scopes.length <= 0) {
        printf("no global scope available\n");
        exit(1);
    }

    return &self->scopes.buffer[0];
}

struct scope *inter_current_scope(struct interpreter *self) {
    if (self->scopes.length <= 0) {
        printf("no scopes available\n");
        exit(1);
    }

    return &self->scopes.buffer[self->scopes.length - 1];
}

struct symbol *inter_find_symbol(struct interpreter *self,
                                 struct string_view name) {
    struct scope *scope = inter_current_scope(self);

    struct symbol *sym = scope_find_symbol(scope, name);
    if (sym) {
        return sym;
    }

    scope = inter_global_scope(self);
    return scope_find_symbol(scope, name);
}

void inter_panic_symbol_not_found(struct string_view name) {
    printf("symbol '");
    fwrite(name.buffer, sizeof(char), name.length, stdin);
    printf("' not found\n");
    exit(1);
}

void inter_panic_symbol_invalid_kind(struct string_view name,
                                     const char *expected) {
    printf("symbol '");
    fwrite(name.buffer, sizeof(char), name.length, stdin);
    printf("' is not a %s\n", expected);
    exit(1);
}

int inter_eval_expression(struct interpreter *self,
                          struct parsed_expression *expr) {
    switch (expr->kind) {
    case PExprNumber:
        return ((struct parsed_expression_number *)expr)->value;
    case PExprIdent: {
        struct string_view ident =
            ((struct parsed_expression_ident *)expr)->ident;

        struct symbol *sym = inter_find_symbol(self, ident);
        if (!sym) {
            inter_panic_symbol_not_found(ident);
        }

        if (sym->kind == SymbolProcedure) {
            inter_panic_symbol_invalid_kind(ident, "variable or constant");
        }

        return sym->value.number;
    }
    case PExprBinOp: {
        struct parsed_expression_binop *binop =
            (struct parsed_expression_binop *)expr;
        int lhs = inter_eval_expression(self, binop->lhs);
        int rhs = inter_eval_expression(self, binop->rhs);

        switch (binop->op) {
        case BinOpAdd:
            return lhs + rhs;
        case BinOpSub:
            return lhs - rhs;
        case BinOpMul:
            return lhs * rhs;
        case BinOpDiv:
            return lhs / rhs;
        }
    }
    }
}

int inter_eval_condition(struct interpreter *self,
                         struct parsed_condition *pcond) {
    switch (pcond->kind) {
    case PCondOdd: {
        struct parsed_expression *expr =
            ((struct parsed_condition_odd *)pcond)->expr;
        return inter_eval_expression(self, expr) % 2 != 0;
    }
    case PCondBinOp: {
        struct parsed_condition_binop *cond =
            (struct parsed_condition_binop *)pcond;

        int lhs = inter_eval_expression(self, cond->lhs);
        int rhs = inter_eval_expression(self, cond->rhs);
        switch (cond->op) {
        case BinOpCompEqual:
            return lhs == rhs;
        case BinOpCompNotEqual:
            return lhs != rhs;
        case BinOpCompLess:
            return lhs < rhs;
        case BinOpCompLessEqual:
            return lhs <= rhs;
        case BinOpCompGreater:
            return lhs > rhs;
        case BinOpCompGreaterEqual:
            return lhs >= rhs;
        }
    }
    }
}

void inter_eval_statement(struct interpreter *self,
                          struct parsed_statement *stmt);

void inter_eval_block(struct interpreter *self, struct parsed_block *block) {
    inter_push_scope(self);

    struct scope *scope = inter_current_scope(self);

    for (int i = 0; i < block->constants.length; i++) {
        struct parsed_const *c = &block->constants.buffer[i];
        list_symbol_push(&scope->symbols,
                         symbol_new_constant(c->name, c->number));
    }

    for (int i = 0; i < block->variables.length; i++) {
        struct parsed_var *c = &block->variables.buffer[i];
        list_symbol_push(&scope->symbols, symbol_new_variable(c->name));
    }

    for (int i = 0; i < block->procedures.length; i++) {
        struct parsed_procedure *c = &block->procedures.buffer[i];
        list_symbol_push(&scope->symbols,
                         symbol_new_procedure(c->name, c->block));
    }

    inter_eval_statement(self, block->stmt);

    inter_pop_scope(self);
}

void inter_eval_statement(struct interpreter *self,
                          struct parsed_statement *pstmt) {
    switch (pstmt->kind) {
    case PStmtEmpty:
        break;
    case PStmtAssign: {
        struct parsed_statement_assign *stmt =
            (struct parsed_statement_assign *)pstmt;

        struct symbol *sym = inter_find_symbol(self, stmt->ident);
        if (!sym) {
            inter_panic_symbol_not_found(stmt->ident);
        }

        if (sym->kind != SymbolVariable) {
            inter_panic_symbol_invalid_kind(stmt->ident, "variable");
        }

        int value = inter_eval_expression(self, stmt->expr);
        sym->value.number = value;
        break;
    }
    case PStmtCall: {
        struct parsed_statement_call *stmt =
            (struct parsed_statement_call *)pstmt;

        struct symbol *sym = inter_find_symbol(self, stmt->ident);
        if (!sym) {
            inter_panic_symbol_not_found(stmt->ident);
        }

        if (sym->kind != SymbolProcedure) {
            inter_panic_symbol_invalid_kind(stmt->ident, "procedure");
        }

        inter_eval_block(self, sym->value.procedure);
        break;
    }
    case PStmtReadln: {
        struct parsed_statement_readln *stmt =
            (struct parsed_statement_readln *)pstmt;

        struct symbol *sym = inter_find_symbol(self, stmt->ident);
        if (!sym) {
            inter_panic_symbol_not_found(stmt->ident);
        }

        if (sym->kind != SymbolVariable) {
            inter_panic_symbol_invalid_kind(stmt->ident, "variable");
        }

        char buffer[64];
        memset(buffer, 0, sizeof(char) * 64);
        fgets(buffer, 60, stdin);
        int length = strlen(buffer);
        fflush(stdin);

        int number = string_view_parse_int(string_view_new(buffer, length));
        sym->value.number = number;
        break;
    }
    case PStmtWriteln: {
        struct parsed_statement_writeln *stmt =
            (struct parsed_statement_writeln *)pstmt;

        int number = inter_eval_expression(self, stmt->expr);
        printf("%d\n", number);
        break;
    }
    case PStmtBegin: {
        struct parsed_statement_begin *stmt =
            (struct parsed_statement_begin *)pstmt;

        for (int i = 0; i < stmt->stmts.length; i++) {
            inter_eval_statement(self, stmt->stmts.buffer[i]);
        }
        break;
    }
    case PStmtIf: {
        struct parsed_statement_if *stmt = (struct parsed_statement_if *)pstmt;

        if (inter_eval_condition(self, stmt->cond)) {
            inter_eval_statement(self, stmt->stmt);
        }
        break;
    }
    case PStmtWhile: {
        struct parsed_statement_while *stmt =
            (struct parsed_statement_while *)pstmt;

        while (inter_eval_condition(self, stmt->cond)) {
            inter_eval_statement(self, stmt->stmt);
        }
        break;
    }
    }
}

void interpreter_eval_program(struct interpreter *self,
                              struct parsed_program *program) {
    inter_eval_block(self, program->block);
}
