#include "token.h"
#include <stdio.h>
#include <stdlib.h>

const char *token_type_name(enum token_type t) {
    switch (t) {
    case TokenInvalid:
        return "Invalid";
    case TokenNumber:
        return "Number";
    case TokenIdent:
        return "Ident";
    case TokenLParen:
        return "LParen";
    case TokenRParen:
        return "RParen";
    case TokenPlus:
        return "Plus";
    case TokenMinus:
        return "Minus";
    case TokenAsterisk:
        return "Asterisk";
    case TokenSlash:
        return "Slash";
    case TokenEqual:
        return "Equal";
    case TokenNotEqual:
        return "NotEqual";
    case TokenLess:
        return "Less";
    case TokenLessEqual:
        return "LessEqual";
    case TokenGreater:
        return "Greater";
    case TokenGreaterEqual:
        return "GreaterEqual";
    case TokenColon:
        return "Colon";
    case TokenComma:
        return "Comma";
    case TokenSemiColon:
        return "SemiColon";
    case TokenDot:
        return "Dot";
    case TokenAssign:
        return "Assign";
    case TokenKeywordOdd:
        return "KeywordOdd";
    case TokenKeywordWhile:
        return "KeywordWhile";
    case TokenKeywordDo:
        return "KeywordDo";
    case TokenKeywordIf:
        return "KeywordIf";
    case TokenKeywordThen:
        return "KeywordThen";
    case TokenKeywordBegin:
        return "KeywordBegin";
    case TokenKeywordEnd:
        return "KeywordEnd";
    case TokenKeywordCall:
        return "KeywordCall";
    case TokenKeywordWriteln:
        return "KeywordWriteln";
    case TokenKeywordReadln:
        return "KeywordReadln";
    case TokenKeywordProcedure:
        return "KeywordProcedure";
    case TokenKeywordVar:
        return "KeywordVar";
    case TokenKeywordConst:
        return "KeywordConst";
    }
}

struct token token_new(enum token_type kind, const char *s, int length) {
    struct token t = {.kind = kind, .value = string_view_new(s, length)};
    return t;
}
