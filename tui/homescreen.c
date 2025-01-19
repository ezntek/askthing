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
#include <termios.h>

#include "../a_string.h"
#include "tui.h"

static void draw_display(TuiHomescreen* s) {
    a_string buf;

    // Welcome to askthing.
    // -------------------
    //
    // what do you want to do?
    //   [load set] [about] [exit] <<-- ONLY DRAW THIS

    printf("  "); // offset
    fflush(stdout);

    switch (s->act) {
        case TUI_HOME_LOAD_SET: {
            buf = astr(S_BGBLUE S_WHITE S_BOLD " load set " S_END
                                               " [about] [exit]");
        } break;
        case TUI_HOME_ABOUT: {
            buf = astr("[load set] " S_BGBLUE S_WHITE S_BOLD " about " S_END
                       " [exit]");
        } break;
        case TUI_HOME_EXIT: {
            buf = astr("[load set] [about] " S_BGBLUE S_WHITE S_BOLD
                       " exit " S_END);
        } break;
    }

    printf("%s", buf.data);

    a_string_free(&buf);
}

static TuiHomescreenCmd get_cmd_with_esc(void) {
    char ch = tui_rgetchar();

    switch (ch) {
        case 'A': {
            // no need up
            return TUI_CMD_NULL;
        } break;
        case 'B': {
            // no need down
            return TUI_CMD_NULL;
        } break;
        case 'C': {
            return TUI_CMD_RIGHT;
        } break;
        case 'D': {
            return TUI_CMD_LEFT;
        } break;
        default: {
            return TUI_CMD_NULL;
        } break;
    }
}

static TuiHomescreenCmd get_cmd(void) {
    char ch = tui_rgetchar(); // get a character in raw mode

    // check if it is an escape sequence
    if (ch == '\033') {
        // C checks both conditions, cannot call tui_rgetchar()
        // as it will consume one unecessary char

        if (tui_rgetchar() == '[')
            return get_cmd_with_esc();
    }

    switch (ch) {
        case '\n': {
            return TUI_CMD_SEL;
        } break;
        default: {
            return TUI_CMD_NULL;
        } break;
    }
}

bool handle_cmd(TuiHomescreen* s) {
    switch (s->cmd) {
        case TUI_CMD_RIGHT: {
            if (s->act + 1 > 2)
                s->act = 0;
            else
                s->act++;
        } break;
        case TUI_CMD_LEFT: {
            if (s->act == 0)
                s->act = 2;
            else
                s->act--;
        } break;
        case TUI_CMD_SEL: {
            return true;
        } break;
        default:
            break;
    }

    return false;
}

TuiHomescreenAction tui_homescreen(void) {
    TuiHomescreen state = {0};
    state.act = TUI_HOME_LOAD_SET;

    do {
        draw_display(&state);

        state.cmd = get_cmd();

        if (handle_cmd(&state))
            break;

        printf(S_CLEAR_LINE);
    } while (state.cmd != TUI_CMD_SEL);

    return state.act;
}
