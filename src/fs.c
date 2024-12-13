#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"

struct string fs_read_file(const char *path) {
    FILE *file = NULL;
    fopen_s(&file, path, "r");

    if (!file)
        return string_new("");

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    char *buffer = malloc(sizeof(char) * (size + 1));
    memset(buffer, 0, sizeof(char) * (size + 1));

    fread(buffer, sizeof(char), size, file);

    struct string str = {.length = size, .buffer = buffer};

    return str;
}
