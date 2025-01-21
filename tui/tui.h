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
    TUI_HOME_ABOUT = 3,
    TUI_HOME_EXIT = 4,
} TuiHomescreenAction;

typedef struct {
    TuiHomescreenAction act;
    TuiHomescreenCmd cmd;
} TuiHomescreen;

TuiHomescreenAction tui_homescreen(void);

void tui_toggle_rawmode(void);
char tui_rgetchar(void);
