#include "cache.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "wordle.h"

struct cache* cache_load(const char* cache_file) {
    
    int fd = open(cache_file, O_RDONLY);

    struct cache* cache = (struct cache*)calloc(sizeof(struct cache), 1);

    if (read(fd, &cache->header, sizeof(struct cache_header)) != sizeof(struct cache_header)) {
        fprintf(stderr, "Failed to parse File\n");
        close(fd);
        _exit(1);
    }

    if (cache->header.magic != MAGIC) {
        fprintf(stderr, "Expected magic values\n");
        close(fd);
        _exit(1);
    }

    cache->words = (char*)malloc(
            sizeof(char) * (cache->header.word_size + 1) *
            cache->header.nb_word
        );
    
    cache->dataset = (char**)malloc(
        sizeof(char*) *
        cache->header.nb_word);

    cache->from_wordle = (char*)malloc(
        sizeof(char) *
        cache->header.nb_word);

    cache->mat = (unsigned char**)malloc(
        sizeof(char) *
        cache->header.nb_word * cache->header.nb_word);
    cache->combis = (int**)malloc(
        sizeof(int) *
        cache->header.nb_word * NB_COMBI);

    read(fd, cache->words, sizeof(char) * (cache->header.word_size + 1) * cache->header.nb_word);
    read(fd, cache->from_wordle, sizeof(char) * cache->header.nb_word);
    read(fd, cache->mat, sizeof(char) * cache->header.nb_word * cache->header.nb_word);
    read(fd, cache->combis, sizeof(int) * cache->header.nb_word * NB_COMBI);

    for (size_t i = 0; i < cache->header.nb_word; i++) {
        cache->dataset[i] = cache->words + i * (cache->header.word_size + 1);
    }

    close(fd);

    return cache;
}


int cache_save(struct cache* cache, const char* path) {
    int fd = open(path, O_WRONLY | O_CREAT);

    write(fd, &cache->header, sizeof(struct cache_header));

    for (size_t i = 0; i < cache->header.nb_word; i++) {
        write(fd, cache->dataset[i], cache->header.word_size + 1);
    }

    write(fd, cache->from_wordle, sizeof(char) * cache->header.nb_word);
    write(fd, cache->mat, sizeof(char) * cache->header.nb_word * cache->header.nb_word);
    write(fd, cache->combis, sizeof(int) * cache->header.nb_word * NB_COMBI);

    close(fd);

    return 0;
}

int compare(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

struct list_word {
    char* data;
    char** dataset;
    size_t nb_word;
    size_t word_size;
};

struct list_word load_list(const char* path) {
    int fd = open(path, O_RDONLY);
    
    char buff[250];

    size_t word_size = 0;
    do {
        read(fd, buff + word_size, 1);
        word_size++;
    }
    while(word_size < 250 && buff[word_size - 1] != '\n');

    word_size--;

    struct stat statbuf;
    fstat(fd, &statbuf);

    size_t nb_word = statbuf.st_size / (word_size + 1);

    if (nb_word * (word_size) < statbuf.st_size)
        nb_word += 1;

    char* data = (char*)calloc(sizeof(char*), nb_word * (word_size + 1));
    char** dataset = (char**) calloc(sizeof(char*), nb_word);

    read(fd, data, statbuf.st_size);

    for (size_t i = 0; i < nb_word; i++) {
        dataset[i] = data + i * (word_size + 1);
        dataset[i][word_size] = 0;
    }

    qsort(dataset, nb_word, sizeof(char*), compare);

    struct list_word ret = {
        .data = data,
        .dataset = dataset,
        .nb_word = nb_word,
        .word_size = word_size
    };
}

struct cache* cache_generate(const char* dataset_path, const char* used_path) {
    struct list_word all = load_list(dataset_path);
    struct list_word used = load_list(used_path);

    assert(all.word_size == used.word_size);

    struct cache* cache = (struct cache*)calloc(sizeof(struct cache), 1);

    cache->header.nb_word = all.nb_word;
    cache->header.word_size = all.word_size;
    cache->words = all.data;
    cache->dataset = all.dataset;
    
    cache->from_wordle = (char*)calloc(sizeof(char), all.nb_word);
    // TODO mat combis
}


