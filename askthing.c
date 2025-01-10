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

#include <stdio.h>

#include "a_string.h"

int main(void) {
    a_string s = astr("hello, world!");
    printf("%s\n", s.data);

    a_string_append(&s, " good morning");
    printf("%s\n", s.data);

    printf("%c\n", a_string_pop(&s));
    a_string_append_char(&s, 'x');
    printf("%s\n", s.data);

    a_string_free(&s);
    return 0;
}
