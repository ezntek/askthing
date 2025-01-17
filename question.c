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
#include <stdio.h>
#include <string.h>

#include "a_string.h"
#include "a_vector.h"
#include "question.h"
#include "util.h"

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

int question_ask(const Question* q, int index) {
    /*
    1. question prompt (for X points):
        (?) answer
    */

    a_string res;
    if (index >= 0) {
        res = a_string_sprintf(S_BLUE "%d. " S_END, index);
    } else {
        res = a_string_new();
    }

    char pts[30];
    if (q->reward == 1) {
        snprintf(pts, sizeof(pts), "1 point");
    } else {
        snprintf(pts, sizeof(pts), "%d points", q->reward);
    }

    char buf[100];
    snprintf(buf, sizeof(buf), "%s " S_DIM "(for " S_END "%s" S_DIM ")" S_END,
             q->prompt.data, pts);
    a_string_append(&res, buf); // `[n]. prompt (for X points)`
    snprintf(buf, sizeof(buf), S_BLUE "\n    (?) " S_END S_DIM);
    a_string_append(&res, buf);

    printf("%s" S_END, res.data);
    fflush(stdout);

    int scanf_result = 0;
    while (scanf_result == 0) {
        scanf_result = scanf("%[^\n]", buf);
        getchar(); // consume newline
        if (scanf_result == 0) {
            while (getchar() != '\n')
                continue;
            printf(S_BOLD S_RED "    Invalid input!\n" S_END);
            printf(S_BLUE "\n    (?) " S_END S_DIM);
            fflush(stdout);
        }
    }

    printf(S_END);
    fflush(stdout);

    a_string raw_answer = astr(buf);
    a_string answer = a_string_trim(&raw_answer);
    a_string_free(&raw_answer);

    bool cond;

    if (!q->case_sensitive) {
        cond = a_string_equal_case_insensitive(&answer, &q->answer);
    } else {
        cond = a_string_equal(&answer, &q->answer);
    }

    int reward;
    if (cond) {
        printf(S_BOLD S_GREEN "    Correct!\n" S_END);
        printf("You get a " S_BOLD S_GREEN "%d" S_END " point reward.\n",
               q->reward);
        reward = q->reward;
    } else {
        printf(S_BOLD S_RED "    Incorrect!\n" S_END);
        printf("You get a " S_BOLD S_RED "no" S_END " reward.\n");
        reward = 0;
    }

    a_string_free(&res);
    a_string_free(&answer);

    return reward;
}

QuestionGroup questiongroup_new(const char* filename) {
    panic("not implemented");
}

QuestionGroup questiongroup_empty(void) {
    return (QuestionGroup){
        .name = astr("Untitled Set"),
        .questions = a_vector_new(),
        .filename = a_string_new(),
        .total_score = 0,
        .fp = NULL,
    };
}

void questiongroup_add_question(QuestionGroup* g, Question q) {
    Question* heapqn = calloc(1, sizeof(Question));
    check_alloc(heapqn);
    *heapqn = q;
    a_vector_append(&g->questions, (void*)heapqn);
}

void questiongroup_open_file(QuestionGroup* g, const char* filename) {
    if (strlen(filename) + 1 > g->filename.cap) {
        a_string_reserve(&g->filename, strlen(filename) + 1);
    }

    strcpy(g->filename.data, filename);

    if (g->write) {
        g->fp = fopen(g->filename.data, "r+");
    } else {
        g->fp = fopen(g->filename.data, "r");
    }

    if (g->fp == NULL) {
        panic("file doesn't exist");
    }
}

void questiongroup_parse_file(QuestionGroup* g) {
    // get the line

    char* fgets_rv;
    const size_t linecap = 250;
    a_string line = a_string_with_capacity(linecap);

    while ((fgets_rv = fgets(line.data, linecap, g->fp)) != NULL) {
        // hacky aah
        line.len = strlen(line.data);

        if (line.len <= 1) {
            continue; // ignore empty lines
        }

        a_string trimmed = a_string_trim(&line);

        if (trimmed.data[0] == '/' && trimmed.data[1] == '/') {
            // comment
            goto loopend;
        }

        // handle the csv part
        //
        // make sure no semicolons are in the name
        if (g->questions.len < 1 && strchr(trimmed.data, ';') == NULL) {
            a_string_copy(&g->name, &trimmed);
            goto loopend;
        }

        a_string question = a_string_with_capacity(70);
        a_string answer = a_string_with_capacity(70);
        int reward = 0;
        char yn = 0; // why cant i scanf with %c lol

        int scanf_result = sscanf(trimmed.data, "%[^;];%[^;];%d;%c[yn]",
                                  question.data, answer.data, &reward, &yn);

        if (scanf_result < 4) {
            panic("whoopsies theres some error in the file");
        }

        question.len = strlen(question.data);
        answer.len = strlen(answer.data);

        Question qn = question_new(question, answer, reward, (yn == 'y'));
        questiongroup_add_question(g, qn);

    loopend:
        a_string_free(&trimmed);
    }

    a_string_free(&line);
}

void questiongroup_destroy(QuestionGroup* g) {
    for (size_t i = 0; i < g->questions.len; i++) {
        question_destroy((Question*)g->questions.data[i]);
        free(g->questions.data[i]);
    }
    a_string_free(&g->name);
    a_vector_free(&g->questions);
    a_string_free(&g->filename);
    fclose(g->fp);
}

void questiongroup_ask(QuestionGroup* g) {
    printf(S_BOLD "%s\n" S_END, g->name.data);
    printf(S_DIM "============\n\n" S_END);

    int total = 0;
    for (size_t i = 0; i < g->questions.len; i++) {
        total += question_ask((Question*)g->questions.data[i], i + 1);
    }

    printf(S_DIM "\n============\n" S_END);
    printf("congratulations! you got " S_GREEN S_BOLD "%d" S_END
           " points in total.\n",
           total);
}
