#pragma once

struct string {
    int length;
    char *buffer;
};

struct string_view {
    int length;
    const char *buffer;
};

struct string string_new(const char *s);
void string_free(struct string self);
struct string_view string_as_view(struct string self);

struct string_view string_view_new(const char *s, int length);
int string_view_parse_int(struct string_view self);
int string_view_compare_chars(struct string_view self, const char *other);
int string_view_compare(struct string_view self, struct string_view other);
