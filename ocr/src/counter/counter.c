#include "counter.h"
#include <stdlib.h>
#include "logger/logger.h"

struct counter *make_counter(int minSize, int maxSize) {
    if (maxSize < minSize) {
        error("Invalid counter initalization: Max size can't be greater than Min Size");
        return NULL;
    }

    struct counter *c = calloc(1, sizeof(struct counter));

    if (!c) {
        fatal("OOM while creating Counter");
        exit(1);
    }

    c->minSize = minSize;
    c->maxSize = maxSize;
    c->numbers = calloc(maxSize - minSize + 1, sizeof(int));

    if (!c->numbers) {
        free(c);
        fatal("OOM while creating Counter numbers field");
        exit(1);
    }

    return c;
}

void add_to_counter(struct counter *c, int size) {
    if (size > c->maxSize || size < c->minSize) {
        warn("Can't add size %d, not in the range [%d, %d]", size, c->minSize, c->maxSize);
        return;
    }

    c->numbers[size - c->minSize]++;
}

int count_of(struct counter *c, int size) {
    if (size > c->maxSize || size < c->minSize) {
        warn("Size %d is not in the counter [%d, %d]", size, c->minSize, c->maxSize);
        return 0;
    }

    return c->numbers[size - c->minSize];
}

void free_counter(struct counter *c) {
    free(c->numbers);
    free(c);
}