#pragma once

#include <memory.h>
#include <stdlib.h>
#include <string.h>

#define DEFINE_LIST(TList, TItem)                                              \
    struct TList {                                                             \
        int length;                                                            \
        int capacity;                                                          \
        TItem *buffer;                                                         \
    };                                                                         \
                                                                               \
    struct TList TList##_new(int cap);                                         \
    void TList##_ensure_capacity(struct TList *self, int cap);                 \
    void TList##_push(struct TList *self, TItem item);                         \
    void TList##_free(struct TList self);

#define IMPL_LIST(TList, TItem)                                                \
    struct TList TList##_new(int cap) {                                        \
        struct TList l;                                                        \
        l.capacity = cap;                                                      \
        l.length = 0;                                                          \
        l.buffer = malloc(sizeof(TItem) * l.capacity);                         \
        memset(l.buffer, 0, sizeof(TItem) * l.capacity);                       \
        return l;                                                              \
    }                                                                          \
                                                                               \
    void TList##_ensure_capacity(struct TList *self, int cap) {                \
        if (self->capacity >= cap)                                             \
            return;                                                            \
                                                                               \
        cap += 4;                                                              \
        self->buffer = realloc(self->buffer, sizeof(TItem) * cap);             \
        self->capacity = cap;                                                  \
    }                                                                          \
                                                                               \
    void TList##_push(struct TList *self, TItem item) {                        \
        TList##_ensure_capacity(self, self->length + 1);                       \
                                                                               \
        self->buffer[self->length] = item;                                     \
        self->length++;                                                        \
    }                                                                          \
                                                                               \
    void TList##_pop(struct TList *self) {                                     \
        if (self->length <= 0) {                                               \
            return;                                                            \
        }                                                                      \
        self->length--;                                                        \
    }                                                                          \
                                                                               \
    void TList##_free(struct TList self) {                                     \
        if (self.buffer)                                                       \
            free(self.buffer);                                                 \
    }
