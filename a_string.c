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
#include <stdlib.h>
#include <string.h>

#include "a_string.h"

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

void a_string_free(a_string* s) {
    if (!a_string_isvalid(s)) {
        panic("you donut the string is invalid");
    }

    free(s->data);

    s->len = -1;
    s->cap = -1;
}

void a_string_reserve(a_string* s, size_t cap) {
    if (!a_string_isvalid(s)) {
        panic("you donut the string is invalid");
    }

    s->data = realloc(s->data, cap);
    check_alloc(s->data);
    s->cap = cap;
}

a_string a_string_from_cstr(const char* cstr) {
    a_string res = {
        .data = NULL,
        .cap = strlen(cstr) + 1,
        .len = strlen(cstr),
    };

    res.data = calloc(res.cap, 1); // sizeof(char)
    strcpy(res.data, cstr);

    return res;
}

a_string astr(const char* cstr) { return a_string_from_cstr(cstr); }

bool a_string_isvalid(const a_string* s) {
    return (s->len == (size_t)-1 || s->cap == (size_t)-1 || s->data == NULL);
}

void a_string_append_char(a_string* s, char c) {
    if (s->len + 2 > s->cap) { // +c +'\0'
        a_string_reserve(s, s->cap * 2);
    }

    s->data[s->len++] = c;
}

void a_string_append_cstr(a_string* s, const char* new) {
    if (s->data == new) {
        panic("identical pointers passed to function");
    }

    size_t required_cap = s->len + strlen(new) + 1;
    if (required_cap > s->cap) {
        while (s->cap < required_cap) {
            a_string_reserve(s, s->cap * 2);
        }
    }

    strcat(s->data, new);
    s->len += strlen(new);
}

void a_string_append_astr(a_string* s, const a_string* new) {
    a_string_append_cstr(s, new->data);
}

void a_string_append(a_string* s, const char* new) {
    a_string_append_cstr(s, new);
}

char a_string_pop(a_string* s) {
    char last = s->data[--s->len];
    s->data[s->len] = '\0';
    return last;
}
