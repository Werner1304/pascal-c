#pragma once

#include "lexer.h"
#include "mystring.h"
#include "parsed_ast.h"
#include "token.h"

struct parser {
    struct lexer lexer;
    struct token current;
};

struct parser parser_new(struct string_view source);
struct parsed_program parser_parse_program(struct parser *self);
