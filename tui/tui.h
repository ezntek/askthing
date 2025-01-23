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

#include <stddef.h>

#include "../a_string.h"
#include "../a_vector.h"

typedef enum {
    TUI_CMD_RIGHT = 0,
    TUI_CMD_LEFT = 1,
    TUI_CMD_SEL = 2,
    TUI_CMD_NULL = 3,
} TuiHomescreenCmd;

typedef enum {
    TUI_HOME_LOAD_SET = 0,
    TUI_HOME_LOAD_FAVORITE = 1,
    TUI_HOME_SAVE_FAVORITE = 2,
    TUI_HOME_DELETE_FAVORITE = 3,
    TUI_HOME_ABOUT = 4,
    TUI_HOME_EXIT = 5,
} TuiHomescreenSelected;

typedef struct {
    TuiHomescreenSelected sel;
    TuiHomescreenCmd cmd;
} TuiHomescreen;

typedef enum {
    TUI_FAV_UP = 0,
    TUI_FAV_DOWN = 1,
    TUI_FAV_SEL = 2,
    TUI_FAV_NULL = 3,
} TuiFavoritesCmd;

typedef struct {
    a_vector* entries;
    a_string hint;
    TuiFavoritesCmd cmd;
    size_t sel_idx;
    size_t max_idx; // max indexed favorite item
} TuiFavorites;

TuiHomescreenSelected tui_homescreen(void);
size_t tui_favorites(a_vector* v, a_string hint);

void tui_toggle_rawmode(void);
char tui_rgetchar(void);
