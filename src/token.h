#pragma once

#include "mystring.h"

enum token_type {
    TokenInvalid,
    TokenNumber,
    TokenIdent,
    TokenLParen,
    TokenRParen,
    TokenPlus,
    TokenMinus,
    TokenAsterisk,
    TokenSlash,
    TokenEqual,
    TokenNotEqual,
    TokenLess,
    TokenLessEqual,
    TokenGreater,
    TokenGreaterEqual,
    TokenColon,
    TokenSemiColon,
    TokenComma,
    TokenDot,
    TokenAssign,
    TokenKeywordOdd,
    TokenKeywordWhile,
    TokenKeywordDo,
    TokenKeywordIf,
    TokenKeywordThen,
    TokenKeywordBegin,
    TokenKeywordEnd,
    TokenKeywordCall,
    TokenKeywordWriteln,
    TokenKeywordReadln,
    TokenKeywordProcedure,
    TokenKeywordVar,
    TokenKeywordConst,
};

const char *token_type_name(enum token_type t);

struct token {
    enum token_type kind;
    struct string_view value;
};

struct token token_new(enum token_type kind, const char *s, int length);
