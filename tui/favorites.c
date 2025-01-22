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
#include "../a_vector.h"
#include "tui.h"

#define OBR S_DIM " [" S_END
#define CBR S_DIM "]" S_END

static void draw_one_line(TuiFavorites* s, size_t idx, bool sel) {
    printf(S_CLEAR_LINE);
    if (idx == s->entries->len) {
        if (sel) {
            printf("    " OBR S_RED S_BOLD "exit" S_END CBR);
        } else {
            printf("    " OBR "exit" CBR);
        }
        return;
    }

    a_string name_disp = a_string_with_capacity(20);
    a_string* name = (a_string*)s->entries->data[idx];

    if (name->len > 20) {
        a_string_ncopy(&name_disp, name, 17);
        a_string_append_cstr(&name_disp, "...");
    } else {
        a_string_copy(&name_disp, name);
    }

    if (sel) {
        printf(S_DIM "%2zu: " S_END OBR S_BLUE S_BOLD "%s" S_END CBR, idx + 1,
               name->data);
        if (s->hint.len != 0) {
            printf(S_DIM " (%s)" S_END, s->hint.data);
        }
    } else {
        printf(S_DIM "%2zu: " S_END OBR "%s" CBR, idx + 1, name_disp.data);
    }

    a_string_free(&name_disp);
}

static void draw_display(TuiFavorites* s) {
    for (size_t i = 0; i <= s->max_idx;
         i++) { // automagically draws the last line (exit)
        printf("    ");

        draw_one_line(s, i, (s->sel_idx == i));
        printf("\n"); // why not
    }
}

static TuiFavoritesCmd get_cmd_with_esc(void) {
    char ch = tui_rgetchar();

    switch (ch) {
        case 'A': {
            // no need up
            return TUI_FAV_UP;
        } break;
        case 'B': {
            return TUI_FAV_DOWN;
        } break;
        case 'C': {
            return TUI_FAV_SEL;
        } break;
        case 'D': {
            return TUI_FAV_NULL;
        } break;
        default: {
        } break;
    }

    return TUI_FAV_NULL;
}

static TuiFavoritesCmd get_cmd(void) {
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
            return TUI_FAV_SEL;
        } break;
        default: {
            return TUI_FAV_NULL;
        } break;
    }
}

static bool handle_cmd(TuiFavorites* s) {
    switch (s->cmd) {
        case TUI_FAV_DOWN: {
            if (s->sel_idx + 1 > s->max_idx)
                s->sel_idx = 0;
            else
                s->sel_idx++;
        } break;
        case TUI_FAV_UP: {
            if (s->sel_idx == 0)
                s->sel_idx = s->max_idx;
            else
                s->sel_idx--;
        } break;
        case TUI_CMD_SEL: {
            return true;
        } break;
        default:
            break;
    }
    return false;
}

size_t tui_favorites(a_vector* v, a_string hint) {
    TuiFavorites state = {
        .entries = v, .max_idx = (v->len <= 8) ? v->len : 8, .hint = hint};

    printf(S_HIDECURSOR);

    do {
        draw_display(&state);

        state.cmd = get_cmd();

        if (handle_cmd(&state))
            break;

        // just go up the number of lines down
        printf("\033[%zuA", state.max_idx + 1);
    } while (state.cmd != TUI_FAV_SEL);

    printf(S_SHOWCURSOR);
    a_string_free(&hint);

    return state.sel_idx;
}
