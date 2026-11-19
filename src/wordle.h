#ifndef WORDLE_H
#define WORDLE_H

#include <stddef.h>
#include <immintrin.h>

#include "vector.h"

#define NB_WORD (size_t) (14855)
#define NB_USED (size_t) 2315

#define NB_COMBI (3 * 3 * 3 * 3 * 3)
#define NB_TENTA 6

#define CLAMP(A, B) (A + (A % B == 0 ? 0 : B - A % B))

#define NB_WORD_PITCH CLAMP(NB_WORD, sizeof(__m256i))
#define NB_COMBI_PITCH CLAMP(NB_COMBI, sizeof(__m256i))

#define WORD_SIZE 5

#define OCR_MAX_PARAMS 16

struct Save
{
    char *available;
    char *from_wordle;

    int best;
    float best_score;

    int prev;
    char prev_ans;

    int turn;
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

struct Param
{
    const char *real_word;
    char automat;
    char nb_thread;
    char disable;
    char limited;
    char prev;
    char rand;
    char debug;
    char* load;
    
    int bench;
    int hard;
    char *ocrArgv[OCR_MAX_PARAMS];
    int ocrArgc;
};

int compact(char a);

// utils.c
void auto_fill(const char* buffer, const char* expect, char* result);

// arg_parser.c
struct Param parse_arg(int argc, char **argv);

// solver.c
unsigned char result_to_char(const char *buffer);
void init(struct Save *data, char *buffer, char *result, struct Param params);
void update_available(struct Save *data, const char *buffer, const char *result);
int update_state(char *result);
int find_best(struct Save *status, int nb_thread);



#endif /* WORDLE_H */
