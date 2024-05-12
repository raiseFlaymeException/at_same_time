#ifndef SSTRING_H
#define SSTRING_H

#include <stdbool.h>
#include <stdio.h>

#include "../array/carray_creator.h"

typedef struct {
    const char *data;
    size_t      size;
} SStringView;

typedef SStringView SStringAll;

CARRAY_CREATE_DECLARATION(SString, char)

// NOLINTBEGIN
void SStringView_init_whole(SStringView *view, const SStringAll *str); 
void SStringView_init_substr(SStringView *view, const SStringAll *str, size_t offset, size_t size);

bool SString_getc_until(SString *str, FILE *file, int char_or_EOF);

void SString_append_str(SString *str, const SStringAll *to_append);

long long SStringAll_findc(const SStringAll *str, char c, size_t offset);
size_t    SStringAll_countc(const SStringAll *str, char c, size_t offset);
// NOLINTEND

#endif // SSTRING_H
#ifdef SSTRING_IMPL
CARRAY_CREATE_DEFINITION(SString, char)

void SStringView_init_whole(SStringView *view, const SStringAll *str) {
    view->data = str->data;
    view->size = str->size;
}
void SStringView_init_substr(SStringView *view, const SStringAll *str, size_t offset, size_t size) {
    view->data = str->data + offset;
    view->size = size;
}

bool SString_getc_until(SString *str, FILE *file, int char_or_eof) {
    int cur_char; // can be EOF so int (because sizeof(EOF)>sizeof(char))
    while (true) {
        cur_char = getc(file);
        if (cur_char == EOF) { return false; }
        if (cur_char == char_or_eof) { return true; }
        SString_append(str, cur_char);
    }
}

void SString_append_str(SString *str, const SStringAll *to_append) {
    for (size_t i = 0; i < to_append->size; i++) { SString_append(str, to_append->data[i]); }
}

long long SStringAll_findc(const SStringAll *str, char chr, size_t offset) {
    for (size_t i = offset; i < str->size; i++) {
        if (str->data[i] == chr) { return i; }
    }
    return -1;
}

size_t SStringAll_countc(const SStringAll *str, char chr, size_t offset) {
    long long size = 0;
    for (size_t i = offset; i < str->size; i++) {
        if (str->data[i] == chr) { size++; }
    }
    return size;
}
#endif // SSTRING_IMPL
