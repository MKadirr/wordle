#ifndef WORDLE_H
#define WORDLE_H

#include <stddef.h>

#define NB_WORD (size_t) 14855
#define NB_USED (size_t) 2315

#define WORD_SIZE 5

struct Save
{
    int *available;
    int *from_wordle;
    int *word_data;
    double *scores;

    int turn;

    char know[WORD_SIZE];
    int misplaced;
    int not_in[WORD_SIZE];
};

struct Score {
    double mean;
    double std;
    double max;
    double E;
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

int compact(char a);

void auto_fill(const char* buffer, const char* expect, char* result);

#endif /* WORDLE_H */
