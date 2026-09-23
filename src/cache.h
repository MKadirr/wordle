#pragma once

#include <stddef.h>

#define MAGIC 0x576F72646C654300L

struct [[gnu::packed]] cache_header {
    unsigned long magic;
    int nb_word;
    size_t word_size;
};

struct [[gnu::packed]] cache {
    struct cache_header header;
    char* words;
    char** dataset;
    char* from_wordle;

    unsigned char** mat;
    int** combis;
};

struct cache* cache_load(const char* cache_file);
int cache_save(struct cache* cache, const char* path);
struct cache* cache_generate(const char* dataset_path, const char* used_path);