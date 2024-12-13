#pragma once

#include "string.h"
#include "token.h"

struct lexer {
    int index;
    char current;
    struct string_view source;
};

struct lexer lexer_new(struct string_view source);
struct token lexer_next(struct lexer *self);
