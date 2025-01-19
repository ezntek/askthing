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

#include "../a_string.h"
#include "tui.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

void tui_toggle_rawmode(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

char tui_rgetchar(void) {
    char res;
    tui_toggle_rawmode();
    res = getchar();
    tui_toggle_rawmode();
    return res;
}
