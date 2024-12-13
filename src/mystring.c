#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "mystring.h"

struct string string_new(const char *s) {
    int length = strlen(s);

    if (length == 0) {
        struct string str = {.length = 0, .buffer = NULL};
        return str;
    }

    char *buffer = malloc(sizeof(char) * (length + 1));
    memset(buffer, 0, sizeof(char) * (length + 1));
    memcpy(buffer, s, sizeof(char) * length);
    struct string str = {.length = length, .buffer = buffer};
    return str;
}

void string_free(struct string self) {
    if (!self.buffer)
        return;

    free(self.buffer);
}

struct string_view string_as_view(struct string self) {
    struct string_view sv = {.buffer = self.buffer, .length = self.length};
    return sv;
}

struct string_view string_view_new(const char *s, int length) {
    struct string_view sv = {.length = length, .buffer = s};
    return sv;
}

int string_view_parse_int(struct string_view self) {
    int sum = 0;

    for (int i = 0; i < self.length; i++) {
        int idx = self.length - i - 1;
        int digit = self.buffer[idx] - '0';
        sum += digit * powl(10, i);
    }

    return sum;
}

int string_view_compare_chars(struct string_view self, const char *other) {
    int other_len = strlen(other);

    if (self.length != other_len) {
        return 0;
    }

    return memcmp(self.buffer, other, self.length) == 0;
}

int string_view_compare(struct string_view self, struct string_view other) {
    if (self.length != other.length) {
        return 0;
    }

    return memcmp(self.buffer, other.buffer, self.length) == 0;
}
