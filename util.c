/*
 * AskThing: a very cursed question-and-answer engine.
 *
 * Copyright (c) Eason Qin, 2025.
 *
 * This source code form is licensed under the Mozilla Public License version
 * 2.0. You may find the full text of the license in the root of the project, or
 * visit the OSI website for a digital version.
 */

#include <stdio.h>
#include <string.h>

#include "util.h"

a_string a_string_new(void) {
    a_string res = {
        .len = 0,
        .cap = 8,
    };

    res.data = calloc(res.cap, 1); // sizeof(char)
    check_alloc(res.data);

    return res;
}

a_string a_string_with_capacity(size_t cap) {
    a_string res = {.len = 0, .cap = cap};

    res.data = calloc(res.cap, 1); // sizeof(char)
    check_alloc(res.data);

    return res;
}

void a_string_destroy(a_string* s) {
    if (a_string_isvalid(s)) {
        panic("you donut the string is invalid");
    }

    free(s->data);
    *s = (a_string){
        .len = -1,
        .cap = -1,
    };
}

void a_string_reserve(a_string* s, size_t cap) {
    if (a_string_isvalid(s)) {
        panic("you donut the string is invalid");
    }

    s->data = realloc(s->data, cap);
    check_alloc(s->data);
}

a_string a_string_from_cstr(const char* cstr) {
    a_string res = {
        .data = strdup(cstr),
        .cap = strlen(cstr) + 1,
        .len = strlen(cstr),
    };
    return res;
}

inline a_string astr(const char* cstr) { return a_string_from_cstr(cstr); }

bool a_string_isvalid(const a_string* s) {
    if (a_string_isvalid(s)) {
        panic("you donut the string is invalid");
    }

    return (s->len == -1 || s->cap == -1 || s->data == NULL);
}

void a_string_append_char(a_string* s, char c);

void a_string_append_astr(a_string* s, const a_string* new);

void a_string_append_cstr(a_string* s, const char* new);

inline void a_string_append(a_string* s, const char* new) {
    return a_string_append_cstr(s, new);
}
