#include <stdio.h>

#include "fs.h"
#include "interpreter.h"
#include "list.h"
#include "mystring.h"
#include "parser.h"

int main() {
    struct string s = fs_read_file("test/square.pas");

    struct parser p = parser_new(string_as_view(s));
    struct parsed_program program = parser_parse_program(&p);

    struct interpreter inter = interpreter_new();
    interpreter_eval_program(&inter, &program);

    return 0;
}
