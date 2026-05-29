#include "vector.h"

#include <stdlib.h>

struct Vector* ctr_vector() {
    struct Vector* v = calloc(1, sizeof(struct Vector));
    init_vector(v);
    return v;
}

void init_vector(struct Vector* v) {
    v->capacity = 5;
    v->size = 0;

    v->data = calloc(5, sizeof(void*));
}

void push_vector(struct Vector* v, void* data) {
    if (v->size == v->capacity) {
        void** tmp = realloc(v->data, v->capacity * 2);
        v->data = tmp;

        v->capacity *= 2;
    }

    v->data[v->size] = data;
    v->size++;
}

void dtr_vector(struct Vector* v) {
    if (v) {
        free(v->data);
        free(v);
    }
}
