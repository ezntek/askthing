/*
 * AskThing: a very cursed question-and-answer engine.
 *
 * Copyright (c) Eason Qin, 2025.
 *
 * This source code form is licensed under the Mozilla Public License version
 * 2.0. You may find the full text of the license in the root of the project, or
 * visit the OSI website for a digital version.
 *
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "util.h"

/**
 * null terminated, heap-allocated string slice.
 */
typedef struct {
    // The raw string slice allocated on the heap.
    char* data;

    // length of the string.
    size_t len;

    // capacity of the string. includes the null terminator.
    size_t cap;
} a_string;

/**
 * non-null-terminated reference to an a_string.
 */
typedef struct {
    // pointer to the beginning of the slize.
    const char* data;

    // length of the slice.
    size_t len;
} a_string_slice;

/**
 * creates an empty a_string.
 */
a_string a_string_new(void);

/**
 * creates an empty a_string with a specified capacity.
 *
 * @param cap the capacity of the string.
 */
a_string a_string_with_capacity(size_t cap);

/**
 * destroys the heap-allocated data in an a_string. Do not read from the
 * a_string after it is destroyed!
 *
 * @param s the string to be destroyed
 */
void a_string_free(a_string* s);

/**
 * reserves a specific capacity on an a_string.
 *
 * @param s the string to be modified
 * @param cap the new capacity of the string
 */
void a_string_reserve(a_string* s, size_t cap);

/**
 * creates an a_string from a C string.
 *
 * @param cstr the C string to be converted. This function does not free the
 * string if it is heap-allocated, the string is instead duplicated.
 */
a_string a_string_from_cstr(const char* cstr);

/**
 * creates an a_string from a C string.
 *
 * shorthand of `a_string_from_cstr()`
 *
 * @param cstr the C string to be converted. This function does not free the
 * string if it is heap-allocated, the string is instead duplicated.
 */
a_string astr(const char* cstr);

/**
 * checks if an a_string is invalid
 *
 * @param s the string to be checked
 */
bool a_string_invalid(const a_string* s);

/**
 * adds 1 character to an a_string
 *
 * @param s the target string to be concatenated
 * @param c the character to be added.
 */
void a_string_append_char(a_string* s, char c);

/**
 * concatenates 2 a_strings together.
 *
 * @param s the target string to be concatenated
 * @param new the string to add on. This string will be kept intact and will not
 * be freed.
 */
void a_string_append_astr(a_string* s, const a_string* new);

/**
 * concatenates a C string to an a_string.
 *
 * @param s the target string to be concatenated
 * @param new the string to add on. This is a C string that will be kept intact.
 */
void a_string_append_cstr(a_string* s, const char* new);

/**
 * concatenates a C string to an a_string.
 *
 * shorthand form of `a_string_append_cstr()`.
 *
 * @param s the target string to be concatenated
 * @param new the string to add on. This string will be kept intact and will not
 * be freed.
 */
void a_string_append(a_string* s, const char* new);

/**
 * removes the last character from an a_string.
 *
 * @param s the target string
 * @return the last character
 */
char a_string_pop(a_string* s);
