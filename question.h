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

#include "a_string.h"

typedef struct {
    a_string prompt;
    a_string answer;
    int reward;
    bool case_sensitive;
} Question;

typedef struct {

} QuestionGroup;

Question question_new(a_string prompt, a_string answer, int reward,
                      bool case_sensitive);
Question question_new_empty(void); // create an empty question
void question_destroy(Question* q);
void question_ask(Question* q);
