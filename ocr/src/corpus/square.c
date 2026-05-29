#include "square.h"
#include <stdlib.h>
#include "logger/logger.h"

struct square *make_square(int startX, int startY, int size, enum color color) {
    struct square *s = calloc(1, sizeof(struct square));

    if (!s) {
        fatal("OOM while creating square");
        exit(1);
    }

    s->startX = startX;
    s->startY = startY;
    s->size = size;
    s->color = color;

    return s;
}

struct square_list *make_square_list(void) {
    struct square_list *s = calloc(1 , sizeof(struct square_list));

    if (!s) {
        fatal("OOM while creating Square list");
        exit(1);
    }

    s->head = NULL;
    s->size = 0;

    return s;
}

struct square_list_element *make_square_list_element(void) {
    struct square_list_element *elm = calloc(1 , sizeof(struct square_list_element));

    if (!elm) {
        fatal("OOM while creating Square list element");
        exit(1);
    }

    elm->data = NULL;
    elm->next = NULL;

    return elm;
}

void free_square(struct square *s) {
    free(s);
}

void free_square_list_element(struct square_list_element *sl) {
    free(sl->data);
    free(sl);
}

void free_square_list(struct square_list *s) {
    struct square_list_element *l = s->head;

    while (l) {
        struct square_list_element *prev = l;
        l = l->next;
        free_square_list_element(prev);
    }

    free(s);
}

void add_square(struct square_list *s, struct square *square) {

    struct square_list_element *elm = make_square_list_element();
    elm->data = square;

    if (!s->head) {
        s->head = elm;
        s->size++;
        return;
    }

    struct square_list_element *l = s->head;
    if (
        l->data->startY > square->startY ||
        (
            l->data->startY == square->startY && l->data->startX > square->startX
        )) {
        s->head = elm;
        elm->next = l;
        s->size++;
        return;
    }

    while (
        l->next && 
        (l->next->data->startY < square->startY ||
        (
            l->next->data->startY == square->startY && 
            l->next->data->startX < square->startX
        ))) {
        l = l->next;
    }

    elm->next = l->next;
    l->next = elm;
    s->size++;
}

struct square *remove_square(struct square_list *s, int startX, int startY) {
    struct square_list_element *l = s->head;

    if (!l) {
        warn("No squares in list but attempt removing a square");
        return NULL;
    }

    if (l->data->startX == startX && l->data->startY == startY) {
        s->head = l->next;
        struct square *square = l->data;
        free(l);
        s->size--;
        return square;
    }

    while(l->next && (l->next->data->startX != startX || l->next->data->startY != startY)) {
        l = l->next;
    }

    if (!l->next) {
        warn("No squares in list with startX %d and startY %d", startX, startY);
        return NULL;
    }

    struct square *square = l->next->data;
    struct square_list_element *se = l->next;
    l->next = l->next->next;
    free(se);
    s->size--;
    return square;
}