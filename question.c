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

#include <stdbool.h>

#include "a_string.h"
#include "question.h"

Question question_new(a_string prompt, a_string answer, int reward,
                      bool case_sensitive) {
    return (Question){prompt, answer, reward, case_sensitive};
}

Question question_new_empty(void) {
    return (Question){a_string_new(), a_string_new(), 0, true};
}

void question_destroy(Question* q) {
    a_string_free(&q->answer);
    a_string_free(&q->prompt);
}
