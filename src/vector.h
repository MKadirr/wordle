#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

struct Vector {
    size_t capacity;
    size_t size;
    void** data;
};

struct Vector* ctr_vector();
void init_vector();

void push_vector(struct Vector* v, void* data);

void dtr_vector(struct Vector* v);

#endif
