#ifndef WORDLE_H
#define WORDLE_H

#include <stddef.h>

#define NB_WORD (size_t) 14855
const char* dataset[];
#define NB_USED (size_t) 2315
const char* used[];

#define WORD_SIZE 5

struct Save
{
    int *available;
    int *word_data;

    int turn;

    char know[5];
    int misplaced;
    int not_in;
};

struct Score {
    double mean;
    double std;
    double max;
};

struct workData {
    // param
    int begin;
    int end;

    struct Save* data;
    
    // output
    double score;
    int idx;
};

struct Input {
    char** dataset;
    size_t dataset_size;
    
    char** wordle_list;
    size_t wordle_list_size;

    int argc;
    char** argv;
};

inline int compact(char a) {
    return 1 << (a % 32);
}

#endif /* WORDLE_H */