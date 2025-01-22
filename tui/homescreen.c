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

#define OBR S_DIM " [" S_END
#define CBR S_DIM "]" S_END

static int prev_sel = -1;

static void draw_display(TuiHomescreen* s) {
    a_string buf;

    // Welcome to askthing.
    // -------------------
    //
    // what do you want to do?
    //   [load set] [about] [exit] <<-- ONLY DRAW THIS

    printf(" "); // offset
    fflush(stdout);

    switch (s->sel) {
        case TUI_HOME_LOAD_SET: {
            buf = astr(OBR S_GREEN S_BOLD
                       "load set" S_END CBR OBR "favorites" CBR OBR
                       "add favorite" CBR OBR "delete favorites" CBR OBR
                       "about" CBR OBR "exit" CBR);
        } break;
        case TUI_HOME_LOAD_FAVORITE: {
            buf =
                astr(OBR "load set" CBR OBR S_GREEN S_BOLD
                         "favorites" S_END CBR OBR "add favorite" CBR OBR
                         "delete favorites" CBR OBR "about" CBR OBR "exit" CBR);
        } break;
        case TUI_HOME_SAVE_FAVORITE: {
            buf =
                astr(OBR "load set" CBR OBR "favorites" CBR OBR S_GREEN S_BOLD
                         "add favorite" S_END CBR OBR "delete favorites" CBR OBR
                         "about" CBR OBR "exit" CBR);
        } break;
        case TUI_HOME_DELETE_FAVORITE: {
            buf = astr(OBR "load set" CBR OBR "favorites" CBR OBR
                           "add favorite" CBR OBR S_GREEN S_BOLD
                           "delete favorites" S_END CBR OBR "about" CBR OBR
                           "exit" CBR);
        } break;
        case TUI_HOME_ABOUT: {
            buf = astr(OBR "load set" CBR OBR "favorites" CBR OBR
                           "add favorite" CBR OBR
                           "delete favorites" CBR OBR S_GREEN S_BOLD
                           "about" S_END CBR OBR "exit" CBR);
        } break;
        case TUI_HOME_EXIT: {
            buf = astr(OBR "load set" CBR OBR "favorites" CBR OBR
                           "add favorite" CBR OBR "delete favorites" CBR OBR
                           "about" CBR OBR S_GREEN S_BOLD "exit" S_END CBR);
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

static bool handle_cmd(TuiHomescreen* s) {
    const size_t MAX = 5;
    switch (s->cmd) {
        case TUI_CMD_RIGHT: {
            if (s->sel + 1 > MAX)
                s->sel = 0;
            else
                s->sel++;
        } break;
        case TUI_CMD_LEFT: {
            if (s->sel == 0)
                s->sel = MAX;
            else
                s->sel--;
        } break;
        case TUI_CMD_SEL: {
            return true;
        } break;
        default:
            break;
    }

    return false;
}

TuiHomescreenSelected tui_homescreen(void) {
    TuiHomescreen state = {.sel = TUI_HOME_LOAD_SET};

    if (prev_sel != -1) {
        state.sel = prev_sel;
    }

    printf(S_HIDECURSOR);

    do {
        draw_display(&state);

        state.cmd = get_cmd();

        if (handle_cmd(&state))
            break;

        printf("\r");
    } while (state.cmd != TUI_CMD_SEL);

    printf(S_SHOWCURSOR);

    prev_sel = state.sel;
    return state.sel;
}
