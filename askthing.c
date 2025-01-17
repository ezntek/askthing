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
#include <string.h>

#include "a_string.h"
#include "a_vector.h"
#include "question.h"
#include "util.h"

int main(int argc, char** argv) {
    argc--;
    argv++;

    a_string filename;
    if (argc >= 1) {
        filename = astr(argv[0]);
    } else {
        printf("enter questions filename: ");
        fflush(stdout);
        a_string rawfn = a_string_with_capacity(100);
        fgets(rawfn.data, 100, stdin);
        rawfn.len = strlen(rawfn.data);
        filename = a_string_trim(&rawfn);
        a_string_free(&rawfn);
    }

    QuestionGroup g = questiongroup_new(filename.data);
    questiongroup_ask(&g);
    questiongroup_destroy(&g);
    a_string_free(&filename);
    return 0;
}
