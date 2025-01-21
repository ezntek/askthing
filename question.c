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

Question question_new(a_string prompt, a_string answers, int reward,
                      bool case_sensitive) {
    Question res = (Question){.prompt = prompt,
                              .answers = a_vector_new(),
                              .reward = reward,
                              .case_sensitive = case_sensitive};
    question_split_answers_to_vector(&res, answers.data);
    a_string_free(&answers);
    return res;
}

Question question_new_empty(void) {
    return (Question){a_string_new(), a_vector_new(), 0, true};
}

void question_destroy(Question* q) {
    for (size_t i = 0; i < q->answers.len; i++) {
        a_string_free((a_string*)q->answers.data[i]);
        free((void*)q->answers
                 .data[i]); // free the heap memory used by the string too
    }

    a_vector_free(&q->answers);
    a_string_free(&q->prompt);
}

void question_split_answers_to_vector(Question* q, const char* answers) {
    char* a = strdup(answers);
    char* token = strtok(a, ",");

    while (token != NULL) {
        a_string answer = astr(token);
        a_string* answer_heap = calloc(1, sizeof(a_string));
        check_alloc(answer_heap);
        *answer_heap = answer;
        a_vector_append(&q->answers, (void*)answer_heap);
        token = strtok(NULL, ",");
    }

    free(a);
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

    int reward;
    for (size_t i = 0; i < q->answers.len; i++) {
        bool cond;
        if (!q->case_sensitive) {
            cond = a_string_equal_case_insensitive(
                &answer, (a_string*)q->answers.data[i]);
        } else {
            cond = a_string_equal(&answer, (a_string*)q->answers.data[i]);
        }

        if (cond) {
            printf(S_BOLD S_GREEN "    Correct!\n" S_END);
            printf("You get a " S_BOLD S_GREEN "%d" S_END " point reward.\n",
                   q->reward);
            reward = q->reward;
            break;
        } else if (!cond && i == q->answers.len - 1) {
            printf(S_BOLD S_RED "    Incorrect!\n" S_END);
            printf("You get a " S_BOLD S_RED "no" S_END " reward.\n");
        }

        reward = 0;
    }

    a_string_free(&res);
    a_string_free(&answer);

    return reward;
}

QuestionGroup questiongroup_new(const char* filename) {
    QuestionGroup res = questiongroup_empty();
    questiongroup_open_file(&res, filename);
    questiongroup_parse_file(&res);
    return res;
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
        fatal("file doesn't exist");
    }

    info(S_DIM "found file at path `%s`." S_END, g->filename.data);
}

void questiongroup_parse_file(QuestionGroup* g) {
    // get the line
    const size_t linecap = 250;
    a_string line = a_string_with_capacity(linecap);

    while (fgets(line.data, linecap, g->fp) != NULL) {
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

        a_string question = a_string_with_capacity(100);
        a_string answers = a_string_with_capacity(200);
        int reward = -1;
        char yn = 'y'; // why cant i scanf with %c lol

        int scanf_result = sscanf(trimmed.data, "%[^;];%[^;];%d;%c[yn]",
                                  question.data, answers.data, &reward, &yn);

        if (scanf_result < 4) {
            fatal_noexit("required fields not found in line `%s`",
                         trimmed.data);

            if (strlen(question.data) == 0) {
                warn("question field not found");
            }

            if (strlen(answers.data) == 0) {
                warn("answers field not found");
            }

            if (reward == -1) {
                warn("reward field not found");
            }

            exit(1);
        }

        question.len = strlen(question.data);
        answers.len = strlen(answers.data);

        Question qn = question_new(question, answers, reward, (yn == 'y'));
        questiongroup_add_question(g, qn);

    loopend:
        a_string_free(&trimmed);
    }

    a_string_free(&line);

    info(S_DIM "loaded file at path `%s`." S_END, g->filename.data);
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
    printf(S_DIM "------------\n" S_END);
    printf(S_BOLD "%s\n" S_END, g->name.data);
    printf(S_DIM "------------\n\n" S_END);

    int total = 0;
    for (size_t i = 0; i < g->questions.len; i++) {
        total += question_ask((Question*)g->questions.data[i], i + 1);
        printf("\n");
    }

    printf(S_DIM "------------\n" S_END);
    printf("congratulations! you got " S_GREEN S_BOLD "%d" S_END
           " points in total.\n",
           total);
}
