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
#include <stdlib.h>

#include "a_vector.h"
#include "util.h"

void print_vector(a_vector* v) {
    for (size_t i = 0; i < v->len; i++) {
        printf("%d, ", *(int*)v->data[i]);
        fflush(stdout);
    }
    printf("\n");
}

int* make_heap_int(int i) {
    int* res = malloc(sizeof(int));
    check_alloc(res);
    *res = i;
    return res;
}

int main(void) {
    int ints[] = {1, 2, 3, 4, 5, 6, 7, 8};
    a_vector v = a_vector_from_ints(ints, 8);
    print_vector(&v);

    a_vector_append(&v, make_heap_int(9));
    a_vector_append(&v, make_heap_int(10));
    print_vector(&v);

    a_vector_free_with_items(&v);
    return 0;
}
