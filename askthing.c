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
#include <stdlib.h>

#include "a_string.h"
#include "a_vector.h"
#include "question.h"
#include "util.h"

int main(void) {
    QuestionGroup qg = questiongroup_empty();
    questiongroup_open_file(&qg, "questions.txt");
    questiongroup_parse_file(&qg);
    questiongroup_destroy(&qg);
    return 69;
}
