#pragma once

#include "list.h"
#include "mystring.h"

enum parsed_expression_kind {
    PExprNumber,
    PExprIdent,
    PExprBinOp,
};

enum parsed_condition_kind {
    PCondOdd,
    PCondBinOp,
};

enum parsed_statement_kind {
    PStmtEmpty,
    PStmtAssign,
    PStmtCall,
    PStmtReadln,
    PStmtWriteln,
    PStmtBegin,
    PStmtIf,
    PStmtWhile,
};

enum binop {
    BinOpAdd,
    BinOpSub,
    BinOpMul,
    BinOpDiv,
};

enum binop_comp {
    BinOpCompEqual,
    BinOpCompNotEqual,
    BinOpCompLess,
    BinOpCompLessEqual,
    BinOpCompGreater,
    BinOpCompGreaterEqual,
};

struct parsed_expression {
    enum parsed_expression_kind kind;
};

struct parsed_expression_number {
    struct parsed_expression base;
    int value;
};

struct parsed_expression_ident {
    struct parsed_expression base;
    struct string_view ident;
};

struct parsed_expression_binop {
    struct parsed_expression base;
    struct parsed_expression *lhs;
    struct parsed_expression *rhs;
    enum binop op;
};

struct parsed_condition {
    enum parsed_condition_kind kind;
};

struct parsed_condition_odd {
    struct parsed_condition base;
    struct parsed_expression *expr;
};

struct parsed_condition_binop {
    struct parsed_condition base;
    struct parsed_expression *lhs;
    struct parsed_expression *rhs;
    enum binop_comp op;
};

struct parsed_statement {
    enum parsed_statement_kind kind;
};

DEFINE_LIST(list_parsed_statement, struct parsed_statement *);

struct parsed_statement_empty {
    struct parsed_statement base;
};

struct parsed_statement_assign {
    struct parsed_statement base;
    struct string_view ident;
    struct parsed_expression *expr;
};

struct parsed_statement_call {
    struct parsed_statement base;
    struct string_view ident;
};

struct parsed_statement_readln {
    struct parsed_statement base;
    struct string_view ident;
};

struct parsed_statement_writeln {
    struct parsed_statement base;
    struct parsed_expression *expr;
};

struct parsed_statement_begin {
    struct parsed_statement base;
    struct list_parsed_statement stmts;
};

struct parsed_statement_if {
    struct parsed_statement base;
    struct parsed_condition *cond;
    struct parsed_statement *stmt;
};

struct parsed_statement_while {
    struct parsed_statement base;
    struct parsed_condition *cond;
    struct parsed_statement *stmt;
};

struct parsed_var {
    struct string_view name;
};

struct parsed_const {
    struct string_view name;
    int number;
};

struct parsed_block;

struct parsed_procedure {
    struct string_view name;
    struct parsed_block *block;
};

DEFINE_LIST(list_parsed_var, struct parsed_var);
DEFINE_LIST(list_parsed_const, struct parsed_const);
DEFINE_LIST(list_parsed_procedure, struct parsed_procedure);

struct parsed_block {
    struct list_parsed_var variables;
    struct list_parsed_const constants;
    struct list_parsed_procedure procedures;
    struct parsed_statement *stmt;
};

struct parsed_program {
    struct parsed_block *block;
};
