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
#include <stdio.h>

#include "a_string.h"
#include "a_vector.h"

typedef struct {
    a_string prompt;
    a_string answer;
    int reward;
    bool case_sensitive;
} Question;

typedef struct {
    a_vector questions; // a_vector of Question
    a_string filename;
    int total_score;
    FILE* fp;
    bool write;
} QuestionGroup;

Question question_new(a_string prompt, a_string answer, int reward,
                      bool case_sensitive);
Question question_new_empty(void); // create an empty question
void question_destroy(Question* q);
int question_ask(const Question* q,
                 int index); // negative index: no index, return: reward

QuestionGroup questiongroup_new(const char* filename);
QuestionGroup questiongroup_empty(void);
void questiongroup_add_question(Question q); // will be slapped on the heap
void questiongroup_open_file(QuestionGroup* g, const char* filename);
void questiongroup_parse_file(
    QuestionGroup*
        g); // fills the existing questiongroup with the file contents
void questiongroup_destroy(QuestionGroup* g);
void questiongroup_ask(QuestionGroup* g);
