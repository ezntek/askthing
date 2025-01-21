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

#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "a_string.h"
#include "a_vector.h"
#include "question.h"
#include "tui/tui.h"
#include "util.h"

#define LICENSE                                                                \
    "This Source Code Form is subject to the terms of the Mozilla Public\n"    \
    "License, v. 2.0. If a copy of the MPL was not distributed with this\n"    \
    "file, You can obtain one at http://mozilla.org/MPL/2.0/."

#define LINES  S_DIM "--------------------" S_END "\n"
#define PROMPT S_BOLD "\nchoose an action.\n" S_END

#define WELCOME                                                                \
    S_BOLD                                                                     \
    "Welcome to askthing.\n" S_END S_DIM "--------------------\n"              \
    "Copyright (c) Eason Qin, 2025.\n" LICENSE S_END "\n"                      \
    "\nversion " VERSION "\n" S_DIM "--------------------\n" S_END PROMPT

#define VERSION "0.1.0"

struct state {
    a_vector favorites; // a_vector of a_string
    FILE* fav_fp;
};

struct state s;
bool running;
struct termios default_termios;

static void ask_question(int argc, char** argv);
static void about(void);
static void handle_exit(void);
static void setup(void);
static a_string ensure_config_dir(void);
static void load_favorites(void);
static void leave(void);

static void view_favorites(void);
static void save_favorite(void);

static void ask_question(int argc, char** argv) {
    a_string filename;
    if (argc >= 1) {
        filename = astr(argv[0]);
    } else {
        printf(S_DIM "enter questions filename: " S_END);
        fflush(stdout);
        a_string rawfn = a_string_with_capacity(100);
        fgets(rawfn.data, 100, stdin);
        rawfn.len = strlen(rawfn.data);
        if (rawfn.len == 1) {
            warn("no file provided");
            a_string_free(&rawfn);
        }
        filename = a_string_trim(&rawfn);
        FILE* tmp = fopen(filename.data, "r");
        if (tmp == NULL) {
            warn("file does not exist");
        } else {
            fclose(tmp);
        }
        a_string_free(&rawfn);
    }

    QuestionGroup g = questiongroup_new(filename.data);
    questiongroup_ask(&g);
    questiongroup_destroy(&g);
    a_string_free(&filename);
}

static void about(void) { printf("version " VERSION "\n"); }

static void handle_ctrlc(int a) {
    a = a; // bypass compiler warning
    running = 0;
    exit(1);
}

static void handle_exit(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_termios);
    printf(S_SHOWCURSOR);
}

static a_string ensure_config_dir(void) {
    char* cfg_dir_raw = getenv("XDG_CONFIG_HOME");
    a_string cfg_dir_path;

    if (cfg_dir_raw == NULL) {
        char* home = getenv("HOME");

        if (home == NULL) {
            fatal("could not find $HOME environment variable");
        }

        cfg_dir_path = a_string_sprintf("%s/.config/askthing", home);
    } else {
        cfg_dir_path = a_string_sprintf("%s/askthing", cfg_dir_raw);
    }

    // sketchy but it works
    DIR* cfg_dir = opendir(cfg_dir_path.data);
    if (cfg_dir != NULL) {
        closedir(cfg_dir);
    } else {
        if (mkdir(cfg_dir_path.data, 0777))
            panic("could not make config dir at `%s`", cfg_dir_path.data);
    }

    info("opened config dir");

    return cfg_dir_path;
}

static void load_favorites(void) {
    a_string cfg_dir = ensure_config_dir();
    a_string favs_file = a_string_sprintf("%s/favorites", cfg_dir.data);

    info("config at: %s", favs_file.data);

    s.fav_fp = fopen(favs_file.data, "r+");

    if (s.fav_fp == NULL) {
        s.fav_fp = fopen(favs_file.data, "w+");
        if (s.fav_fp == NULL) {
            panic("could not open favorites file for w+");
        }
    } else {
        a_string line = a_string_with_capacity(200);
        while (fgets(line.data, 200, s.fav_fp) != NULL) {
            line.len = strlen(line.data);
            a_string new = a_string_trim(&line);

            if (new.len == 0)
                continue;

            a_string* heapstr = calloc(1, sizeof(a_string));
            check_alloc(heapstr);
            *heapstr = new;
            a_vector_append(&s.favorites, (void*)heapstr);
        }

        info("loaded favorites");
        a_string_free(&line);
    }

    a_string_free(&favs_file);
    a_string_free(&cfg_dir);
}

static void view_favorites(void) {
    size_t index = tui_favorites(&s.favorites);
    a_string* fn = s.favorites.data[index];
    if (index == s.favorites.len) {
        info("left with exit");
    } else {
        info("got `%s`", fn->data);
    }
}

static void save_favorite(void) {
    printf(S_DIM "add a favorite: " S_END);
    fflush(stdout);
    a_string rawfn = a_string_with_capacity(100);
    fgets(rawfn.data, 100, stdin);
    rawfn.len = strlen(rawfn.data);
    if (rawfn.len == 1) {
        fatal("no file provided");
        a_string_free(&rawfn);
    }
    a_string filename = a_string_trim(&rawfn);
    a_string_free(&rawfn);

    FILE* tmp = fopen(filename.data, "r");
    if (tmp != NULL) {
        fclose(tmp);

        fputs(filename.data, s.fav_fp);
        fputs("\n", s.fav_fp);

        a_string* fnheap = calloc(1, sizeof(a_string));
        check_alloc(fnheap);
        *fnheap = filename;
        a_vector_append(&s.favorites, (void*)fnheap);
    } else {
        warn("file does not exist!");
        a_string_free(&filename);
    }
}

static void setup(void) {
    running = 1;
    s.favorites = a_vector_new();
    load_favorites();

    tcgetattr(STDIN_FILENO, &default_termios);
    signal(SIGINT, handle_ctrlc);
    atexit(handle_exit);
    printf(WELCOME);
}

static void leave(void) {

    for (size_t i = 0; i < s.favorites.len; i++) {
        a_string_free((a_string*)s.favorites.data[i]);
        free(s.favorites.data[i]);
    }
    a_vector_free(&s.favorites);

    if (s.fav_fp != NULL) {
        fclose(s.fav_fp);
    }

    handle_exit();
    printf(S_BOLD "Bye\n" S_END);
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    argc--;
    argv++;

    setup();

    TuiHomescreenSelected a;

    do {
        a = tui_homescreen();
        printf("\n\n");
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_termios);
        switch (a) {
            case TUI_HOME_LOAD_SET: {
                ask_question(argc, argv);
            } break;
            case TUI_HOME_LOAD_FAVORITE: {
                view_favorites();
            } break;
            case TUI_HOME_SAVE_FAVORITE: {
                save_favorite();
            } break;
            case TUI_HOME_ABOUT: {
                about();
            } break;
            case TUI_HOME_EXIT: {
                running = 0;
                leave();
            }
        }
        printf(PROMPT);
    } while (running);

    leave();
}
