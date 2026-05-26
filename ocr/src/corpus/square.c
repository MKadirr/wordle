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

void free_square(struct square *s) {
    free(s);
}