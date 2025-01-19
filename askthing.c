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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "a_string.h"
#include "question.h"
#include "tui/tui.h"
#include "util.h"

#define LICENSE                                                                \
    "This Source Code Form is subject to the terms of the Mozilla Public\n"    \
    "License, v. 2.0. If a copy of the MPL was not distributed with this\n"    \
    "file, You can obtain one at http://mozilla.org/MPL/2.0/."

#define WELCOME                                                                \
    S_BGBLUE S_WHITE S_BOLD                                                    \
        "Welcome to askthing.\n" S_END S_DIM "--------------------\n"          \
        "Copyright (c) Eason Qin, 2025.\n" LICENSE S_END "\n"                  \
        "\nversion " VERSION "\n" S_DIM "--------------------\n\n" S_END       \
        "what do you want to do today?\n"

#define VERSION "0.1.0"
bool running;
struct termios default_termios;

static void ask_question(int argc, char** argv) {
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
    nanosleep(&(struct timespec){.tv_sec = 2}, NULL);
}

static void about(void) {
    printf("version " VERSION "\n");
    nanosleep(&(struct timespec){.tv_sec = 2}, NULL);
}

static void handle_ctrlc(int dummy) { running = 0; }

int main(int argc, char** argv) {
    argc--;
    argv++;

    signal(SIGINT, handle_ctrlc);
    running = 1;

    tcgetattr(STDIN_FILENO, &default_termios);

    printf(S_ENTER_ALT S_CLEAR_SCREEN);
    printf(WELCOME);

    TuiHomescreenAction a;

    do {
        a = tui_homescreen();
        printf("\n\n");
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_termios);
        switch (a) {
            case TUI_HOME_LOAD_SET: {
                ask_question(argc, argv);
            } break;
            case TUI_HOME_ABOUT: {
                about();
            } break;
            case TUI_HOME_EXIT:
                goto leave;
        }

        printf(S_CLEAR_SCREEN WELCOME);
    } while (running);

leave:
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_termios);
    printf(S_LEAVE_ALT);
    printf(S_BOLD "Bye\n" S_END);
}
