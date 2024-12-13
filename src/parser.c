#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

struct parser parser_new(struct string_view source) {
    struct parser p = {
        .lexer = lexer_new(source),
        .current = token_new(TokenInvalid, "", 0),
    };

    return p;
}

struct parsed_expression *parser_parse_expression(struct parser *self);

int parser_try_consume(struct parser *self, enum token_type tt) {
    if (self->current.kind != tt) {
        return 0;
    }

    self->current = self->current = lexer_next(&self->lexer);
    return 1;
}

struct string_view parser_consume(struct parser *self, enum token_type tt) {
    struct string_view v = self->current.value;

    if (!parser_try_consume(self, tt)) {
        printf("unexpected token %s expected %s\n",
               token_type_name(self->current.kind), token_type_name(tt));
        exit(1);
    }

    return v;
}

struct parsed_expression *parser_parse_factor(struct parser *self) {
    switch (self->current.kind) {
    case TokenNumber: {
        struct string_view v = parser_consume(self, TokenNumber);
        int number = string_view_parse_int(v);
        struct parsed_expression_number *expr =
            malloc(sizeof(struct parsed_expression_number));
        expr->base.kind = PExprNumber;
        expr->value = number;
        return (struct parsed_expression *)expr;
    }
    case TokenIdent: {
        struct string_view v = parser_consume(self, TokenIdent);
        struct parsed_expression_ident *expr =
            malloc(sizeof(struct parsed_expression_ident));
        expr->base.kind = PExprIdent;
        expr->ident = v;
        return (struct parsed_expression *)expr;
    }
    case TokenLParen: {
        parser_consume(self, TokenLParen);
        struct parsed_expression *expr = parser_parse_expression(self);
        parser_consume(self, TokenRParen);
        return expr;
    }
    default:
        printf("parse_factor: unexpected token %s\n",
               token_type_name(self->current.kind));
        exit(1);
        return NULL;
    }
}

struct parsed_expression *parser_parse_term(struct parser *self) {
    struct parsed_expression *lhs = parser_parse_factor(self);

    while (self->current.kind == TokenAsterisk ||
           self->current.kind == TokenSlash) {
        switch (self->current.kind) {
        case TokenAsterisk: {
            parser_consume(self, TokenAsterisk);
            struct parsed_expression *rhs = parser_parse_factor(self);
            struct parsed_expression_binop *expr =
                malloc(sizeof(struct parsed_expression_binop));
            expr->base.kind = PExprBinOp;
            expr->lhs = lhs;
            expr->rhs = rhs;
            expr->op = BinOpMul;
            lhs = (struct parsed_expression *)expr;
            break;
        }
        case TokenSlash: {
            parser_consume(self, TokenSlash);
            struct parsed_expression *rhs = parser_parse_factor(self);
            struct parsed_expression_binop *expr =
                malloc(sizeof(struct parsed_expression_binop));
            expr->base.kind = PExprBinOp;
            expr->lhs = lhs;
            expr->rhs = rhs;
            expr->op = BinOpDiv;
            lhs = (struct parsed_expression *)expr;
            break;
        }
        default:
            break;
        }
    }

    return lhs;
}

struct parsed_expression *parser_parse_expression(struct parser *self) {
    struct parsed_expression *lhs = parser_parse_term(self);

    while (self->current.kind == TokenPlus ||
           self->current.kind == TokenMinus) {
        switch (self->current.kind) {
        case TokenPlus: {
            parser_consume(self, TokenPlus);
            struct parsed_expression *rhs = parser_parse_term(self);
            struct parsed_expression_binop *expr =
                malloc(sizeof(struct parsed_expression_binop));
            expr->base.kind = PExprBinOp;
            expr->lhs = lhs;
            expr->rhs = rhs;
            expr->op = BinOpAdd;
            lhs = (struct parsed_expression *)expr;
            break;
        }
        case TokenMinus: {
            parser_consume(self, TokenMinus);
            struct parsed_expression *rhs = parser_parse_term(self);
            struct parsed_expression_binop *expr =
                malloc(sizeof(struct parsed_expression_binop));
            expr->base.kind = PExprBinOp;
            expr->lhs = lhs;
            expr->rhs = rhs;
            expr->op = BinOpSub;
            lhs = (struct parsed_expression *)expr;
            break;
        }
        default:
            break;
        }
    }

    return lhs;
}

struct parsed_condition *parser_parse_condition(struct parser *self) {
    if (parser_try_consume(self, TokenKeywordOdd)) {
        struct parsed_condition_odd *odd =
            malloc(sizeof(struct parsed_condition_odd));
        odd->base.kind = PCondOdd;
        odd->expr = parser_parse_expression(self);
        return (struct parsed_condition *)odd;
    }

    struct parsed_expression *lhs = parser_parse_expression(self);

    enum binop_comp op = BinOpCompEqual;
    switch (self->current.kind) {
    case TokenEqual:
        op = BinOpCompEqual;
        break;
    case TokenNotEqual:
        op = BinOpCompNotEqual;
        break;
    case TokenLess:
        op = BinOpCompLess;
        break;
    case TokenLessEqual:
        op = BinOpCompLessEqual;
        break;
    case TokenGreater:
        op = BinOpCompGreater;
        break;
    case TokenGreaterEqual:
        op = BinOpCompGreaterEqual;
        break;
    default:
        printf("parse_condition: unexpected token %s\n",
               token_type_name(self->current.kind));
        exit(1);
        return NULL;
    }

    parser_consume(self, self->current.kind);

    struct parsed_expression *rhs = parser_parse_expression(self);

    struct parsed_condition_binop *cond =
        malloc(sizeof(struct parsed_condition_binop));
    cond->base.kind = PCondBinOp;
    cond->lhs = lhs;
    cond->rhs = rhs;
    cond->op = op;
    return (struct parsed_condition *)cond;
}

struct list_parsed_const parser_parse_contants(struct parser *self) {
    struct list_parsed_const list = list_parsed_const_new(4);

    if (!parser_try_consume(self, TokenKeywordConst)) {
        return list;
    }

    while (self->current.kind != TokenSemiColon) {
        struct string_view ident = parser_consume(self, TokenIdent);
        parser_consume(self, TokenEqual);
        struct string_view number_sv = parser_consume(self, TokenNumber);
        int number = string_view_parse_int(number_sv);

        struct parsed_const c = {.name = ident, .number = number};
        list_parsed_const_push(&list, c);

        if (!parser_try_consume(self, TokenComma)) {
            break;
        }
    }

    parser_consume(self, TokenSemiColon);

    return list;
}

struct list_parsed_var parser_parse_variables(struct parser *self) {
    struct list_parsed_var list = list_parsed_var_new(4);

    if (!parser_try_consume(self, TokenKeywordVar)) {
        return list;
    }

    while (self->current.kind != TokenSemiColon) {
        struct string_view ident = parser_consume(self, TokenIdent);

        struct parsed_var v = {.name = ident};
        list_parsed_var_push(&list, v);

        if (!parser_try_consume(self, TokenComma)) {
            break;
        }
    }

    parser_consume(self, TokenSemiColon);

    return list;
}

struct parsed_statement *parser_parse_statement(struct parser *self) {
    switch (self->current.kind) {
    case TokenIdent: {
        struct parsed_statement_assign *stmt =
            malloc(sizeof(struct parsed_statement_assign));
        stmt->base.kind = PStmtAssign;

        struct string_view ident = parser_consume(self, TokenIdent);
        parser_consume(self, TokenAssign);
        struct parsed_expression *expr = parser_parse_expression(self);

        stmt->ident = ident;
        stmt->expr = expr;

        return (struct parsed_statement *)stmt;
    }
    case TokenKeywordCall: {
        struct parsed_statement_call *stmt =
            malloc(sizeof(struct parsed_statement_call));
        stmt->base.kind = PStmtCall;

        parser_consume(self, TokenKeywordCall);
        struct string_view ident = parser_consume(self, TokenIdent);

        stmt->ident = ident;

        return (struct parsed_statement *)stmt;
    }
    case TokenKeywordReadln: {
        struct parsed_statement_readln *stmt =
            malloc(sizeof(struct parsed_statement_readln));
        stmt->base.kind = PStmtReadln;

        parser_consume(self, TokenKeywordReadln);
        struct string_view ident = parser_consume(self, TokenIdent);

        stmt->ident = ident;

        return (struct parsed_statement *)stmt;
    }
    case TokenKeywordWriteln: {
        struct parsed_statement_writeln *stmt =
            malloc(sizeof(struct parsed_statement_writeln));
        stmt->base.kind = PStmtWriteln;

        parser_consume(self, TokenKeywordWriteln);
        struct parsed_expression *expr = parser_parse_expression(self);

        stmt->expr = expr;

        return (struct parsed_statement *)stmt;
    }
    case TokenKeywordBegin: {
        struct parsed_statement_begin *stmt =
            malloc(sizeof(struct parsed_statement_begin));
        stmt->base.kind = PStmtBegin;

        parser_consume(self, TokenKeywordBegin);

        struct list_parsed_statement stmts = list_parsed_statement_new(4);
        list_parsed_statement_push(&stmts, parser_parse_statement(self));

        while (self->current.kind == TokenSemiColon) {
            parser_consume(self, TokenSemiColon);
            struct parsed_statement *s = parser_parse_statement(self);
            list_parsed_statement_push(&stmts, s);
        }

        parser_consume(self, TokenKeywordEnd);

        stmt->stmts = stmts;

        return (struct parsed_statement *)stmt;
    }
    case TokenKeywordIf: {
        struct parsed_statement_if *stmt =
            malloc(sizeof(struct parsed_statement_if));
        stmt->base.kind = PStmtIf;

        parser_consume(self, TokenKeywordIf);
        struct parsed_condition *cond = parser_parse_condition(self);

        parser_consume(self, TokenKeywordThen);
        struct parsed_statement *s = parser_parse_statement(self);

        stmt->cond = cond;
        stmt->stmt = s;

        return (struct parsed_statement *)stmt;
    }
    case TokenKeywordWhile: {
        struct parsed_statement_if *stmt =
            malloc(sizeof(struct parsed_statement_if));
        stmt->base.kind = PStmtWhile;

        parser_consume(self, TokenKeywordWhile);
        struct parsed_condition *cond = parser_parse_condition(self);

        parser_consume(self, TokenKeywordDo);
        struct parsed_statement *s = parser_parse_statement(self);

        stmt->cond = cond;
        stmt->stmt = s;

        return (struct parsed_statement *)stmt;
    }
    default: {
        struct parsed_statement_empty *stmt =
            malloc(sizeof(struct parsed_statement_empty));
        stmt->base.kind = PStmtEmpty;
        return (struct parsed_statement *)stmt;
    }
    }
}

struct parsed_procedure parser_parse_procedure(struct parser *self);

struct parsed_block *parser_parse_block(struct parser *self) {
    struct list_parsed_const constants = parser_parse_contants(self);
    struct list_parsed_var vars = parser_parse_variables(self);

    struct list_parsed_procedure procs = list_parsed_procedure_new(2);
    while (self->current.kind == TokenKeywordProcedure) {
        list_parsed_procedure_push(&procs, parser_parse_procedure(self));
    }

    struct parsed_statement *stmt = parser_parse_statement(self);

    struct parsed_block *block = malloc(sizeof(struct parsed_block));
    block->constants = constants;
    block->variables = vars;
    block->procedures = procs;
    block->stmt = stmt;
    return block;
}

struct parsed_procedure parser_parse_procedure(struct parser *self) {
    parser_consume(self, TokenKeywordProcedure);
    struct string_view ident = parser_consume(self, TokenIdent);
    parser_consume(self, TokenSemiColon);
    struct parsed_block *block = parser_parse_block(self);
    parser_consume(self, TokenSemiColon);

    struct parsed_procedure proc = {
        .name = ident,
        .block = block,
    };
    return proc;
}

struct parsed_program parser_parse_program(struct parser *self) {
    self->current = lexer_next(&self->lexer);

    struct parsed_program pp;
    pp.block = parser_parse_block(self);
    parser_consume(self, TokenDot);
    return pp;
}
