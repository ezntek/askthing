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

#define check_alloc(ptr)                                                       \
                                                                               \
    if (ptr == NULL) {                                                         \
        panic("allocation of `%s` failed", #ptr);                              \
        perror("perror");                                                      \
        exit(1);                                                               \
    }

#define eprintf(format, ...) fprintf(stderr, format, ##__VA_ARGS__);

#define panic(...)                                                             \
    {                                                                          \
        eprintf("\033[31;1mpanic:\033[0m line `%d`, func `%s` in file `%s`: "  \
                "`",                                                           \
                __LINE__, __func__, __FILE__);                                 \
        eprintf(__VA_ARGS__);                                                  \
        eprintf("`\n");                                                        \
        exit(1);                                                               \
    }
