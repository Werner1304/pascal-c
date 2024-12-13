#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

struct lexer lexer_new(struct string_view source) {
    struct lexer l = {.index = 0, .current = '\0', .source = source};

    if (source.length <= 0) {
        return l;
    }

    l.current = l.source.buffer[0];
    return l;
}

void lexer_advance(struct lexer *self) {
    self->index++;

    if (self->index >= self->source.length) {
        self->current = '\0';
    } else {
        self->current = self->source.buffer[self->index];
    }
}

char lexer_peek(struct lexer *self) {
    int index = self->index + 1;

    if (index >= self->source.length) {
        return '\0';
    } else {
        return self->source.buffer[index];
    }
}

void lexer_skip_space(struct lexer *self) {
    while (isspace(self->current)) {
        lexer_advance(self);
    }
}

struct token lexer_lex_number(struct lexer *self) {
    const char *start = self->source.buffer + self->index;
    int length = 0;

    while (isdigit(self->current)) {
        lexer_advance(self);
        length++;
    }

    return token_new(TokenNumber, start, length);
}

enum token_type lexer_get_single_char_token(char c) {
    switch (c) {
    case '(':
        return TokenLParen;
    case ')':
        return TokenRParen;
    case ',':
        return TokenComma;
    case '.':
        return TokenDot;
    case ':':
        return TokenColon;
    case ';':
        return TokenSemiColon;
    case '+':
        return TokenPlus;
    case '-':
        return TokenMinus;
    case '*':
        return TokenAsterisk;
    case '/':
        return TokenSlash;
    case '=':
        return TokenEqual;
    case '#':
        return TokenNotEqual;
    case '<':
        return TokenLess;
    case '>':
        return TokenGreater;
    default:
        return TokenInvalid;
    }
}

enum token_type lexer_get_double_char_token(char c, char d) {
    if (c == ':' && d == '=')
        return TokenAssign;

    if (c == '<' && d == '=')
        return TokenLessEqual;

    if (c == '>' && d == '=')
        return TokenGreaterEqual;

    return TokenInvalid;
}

enum token_type lexer_get_keyword(struct string_view s) {
    if (string_view_compare_chars(s, "begin")) {
        return TokenKeywordBegin;
    }

    if (string_view_compare_chars(s, "call")) {
        return TokenKeywordCall;
    }

    if (string_view_compare_chars(s, "const")) {
        return TokenKeywordConst;
    }

    if (string_view_compare_chars(s, "do")) {
        return TokenKeywordDo;
    }

    if (string_view_compare_chars(s, "end")) {
        return TokenKeywordEnd;
    }

    if (string_view_compare_chars(s, "if")) {
        return TokenKeywordIf;
    }

    if (string_view_compare_chars(s, "odd")) {
        return TokenKeywordOdd;
    }

    if (string_view_compare_chars(s, "procedure")) {
        return TokenKeywordProcedure;
    }

    if (string_view_compare_chars(s, "readln")) {
        return TokenKeywordReadln;
    }

    if (string_view_compare_chars(s, "then")) {
        return TokenKeywordThen;
    }

    if (string_view_compare_chars(s, "var")) {
        return TokenKeywordVar;
    }

    if (string_view_compare_chars(s, "while")) {
        return TokenKeywordWhile;
    }

    if (string_view_compare_chars(s, "writeln")) {
        return TokenKeywordWriteln;
    }

    return TokenInvalid;
}

struct token lexer_lex_ident(struct lexer *self) {
    const char *start = self->source.buffer + self->index;
    int length = 0;

    while (self->current == '_' || isalnum(self->current)) {
        lexer_advance(self);
        length++;
    }

    struct string_view sv = string_view_new(start, length);

    enum token_type tt = lexer_get_keyword(sv);

    if (tt != TokenInvalid) {
        return token_new(tt, start, length);
    }

    return token_new(TokenIdent, start, length);
}

struct token lexer_next(struct lexer *self) {
    while (self->current != '\0') {
        if (isspace(self->current)) {
            lexer_skip_space(self);
        } else if (isdigit(self->current)) {
            return lexer_lex_number(self);
        } else if (self->current == '_' || isalpha(self->current)) {
            return lexer_lex_ident(self);
        } else if (lexer_get_double_char_token(
                       self->current, lexer_peek(self)) != TokenInvalid) {
            const char *start = self->source.buffer + self->index;
            enum token_type tt =
                lexer_get_double_char_token(self->current, lexer_peek(self));
            lexer_advance(self);
            lexer_advance(self);
            return token_new(tt, start, 2);
        } else if (lexer_get_single_char_token(self->current)) {
            const char *start = self->source.buffer + self->index;
            enum token_type tt = lexer_get_single_char_token(self->current);
            lexer_advance(self);
            return token_new(tt, start, 1);
        } else {
            printf("invalid character '%c'\n", self->current);
            exit(1);
        }
    }

    return token_new(TokenInvalid, "", 0);
}
