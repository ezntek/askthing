/*
 * Copyright (c) Eason Qin, 2025.
 *
 * This source code form is licensed under the Mozilla Public License version
 * 2.0. You may find the full text of the license in the root of the project, or
 * visit the OSI website for a digital version.
 */

#include <ctype.h>
#include <stdarg.h>
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

void a_string_clear(a_string* s) { memset(s->data, '\0', s->cap); }

void a_string_free(a_string* s) {
    if (a_string_invalid(s)) {
        panic("you donut the string is invalid");
    }

    free(s->data);

    s->len = -1;
    s->cap = -1;
}

void a_string_copy(a_string* dest, const a_string* src) {
    if (src->len > dest->cap) {
        a_string_reserve(dest, src->cap);
    }

    strncpy(dest->data, src->data, src->len);
}

void a_string_copy_cstr(a_string* dest, const char* src) {
    size_t len = strlen(src);
    if (len + 1 > dest->cap) {
        a_string_reserve(dest, len);
    }

    strncpy(dest->data, src, len);
}

void a_string_ncopy(a_string* dest, const a_string* src, size_t chars) {
    if (chars > dest->cap) {
        a_string_reserve(dest, chars);
    }

    strncpy(dest->data, src->data, chars);
}

void a_string_reserve(a_string* s, size_t cap) {
    if (a_string_invalid(s)) {
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

a_string a_string_duplicate(const a_string* s) {
    a_string res = a_string_with_capacity(s->cap);
    res.len = s->len;
    strncpy(res.data, s->data, s->len);
    return res;
}

a_string a_string_sprintf(const char* restrict format, ...) {
    va_list args;
    va_start(args, format);
    char* dest = NULL;
    vasprintf(&dest, format, args);
    va_end(args);
    a_string res = a_string_from_cstr(dest);
    free(dest);
    return res;
}

bool a_string_invalid(const a_string* s) {
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

a_string a_string_trim_left(const a_string* s) {
    size_t i = 0;
    while (i < s->len) { // must check this first, else segfault
        if (strchr(" \n\t\r", s->data[i])) {
            i++;
        } else {
            break;
        }
    }

    return a_string_from_cstr(&s->data[i]);
}

a_string a_string_trim_right(const a_string* s) {
    size_t end = s->len - 1;
    while (end >= 0) {
        if (strchr(" \n\t\r", s->data[end])) {
            end--;
        } else {
            break;
        }
    }
    end++;

    a_string res = a_string_with_capacity(end + 1);
    for (size_t i = 0; i < end; i++) {
        res.data[i] = s->data[i];
    }

    return res;
}

a_string a_string_trim(const a_string* s) {
    size_t begin = 0;
    size_t end = s->len - 1;

    while (begin < s->len) { // must check this first, else segfault
        if (strchr(" \n\t\r", s->data[begin]) != NULL) {
            begin++;
        } else {
            break;
        }
    }

    if (end == s->len)
        end--;

    while (end >= 0) {
        if (strchr(" \n\t\r", s->data[end]) != NULL) {
            end--;
        } else {
            break;
        }
    }
    end++;

    a_string res = a_string_with_capacity(end - begin + 1);
    res.len = end - begin;

    size_t i = 0;
    for (size_t j = begin; j < end; j++) {
        res.data[i++] = s->data[j];
    }

    return res;
}

a_string a_string_toupper(const a_string* s) {
    a_string res = a_string_with_capacity(s->cap);
    for (size_t i = 0; i < s->len; i++) {
        res.data[i] = toupper(s->data[i]);
    }
    return res;
}

a_string a_string_tolower(const a_string* s) {
    a_string res = a_string_with_capacity(s->cap);
    for (size_t i = 0; i < s->len; i++) {
        res.data[i] = tolower(s->data[i]);
    }
    return res;
}

void a_string_inplace_toupper(a_string* s) {
    a_string res = a_string_with_capacity(s->cap);
    for (size_t i = 0; i < s->len; i++) {
        s->data[i] = tolower(s->data[i]);
    }
}

void a_string_inplace_tolower(a_string* s) {
    for (size_t i = 0; i < s->len; i++) {
        s->data[i] = tolower(s->data[i]);
    }
}

bool a_string_equal(const a_string* lhs, const a_string* rhs) {
    if (lhs->len != rhs->len) {
        return false;
    }

    for (size_t i = 0; i < lhs->len; i++) {
        if (lhs->data[i] != rhs->data[i]) {
            return false;
        }
    }

    return true;
}

bool a_string_equal_case_insensitive(const a_string* lhs, const a_string* rhs) {
    if (lhs->len != rhs->len) {
        return false;
    }

    for (size_t i = 0; i < lhs->len; i++) {
        if (tolower(lhs->data[i]) != tolower(rhs->data[i])) {
            return false;
        }
    }

    return true;
}
