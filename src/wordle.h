#ifndef WORDLE_H
#define WORDLE_H

#include <stddef.h>

#include "vector.h"

// default number of word available as an answer
#define NB_WORD (size_t) (14855)
// number of word from "official" wordle list
#define NB_USED (size_t) 2315

// type withing wich we store available data.
#define PACKED_TYPE unsigned long long

// size in byte of packed word
#define SIZEOF_PACKED (sizeof(PACKED_TYPE)) // 8

#define WORD_PAR_PACKED (SIZEOF_PACKED * 8) // 64

// Clamp to calculate the numner of bytes required to pack all word
#define CLAMP(A, B) (A + (A % B == 0 ? 0 : B - (A % B)))

// nb of word aligned to the size of packed type
#define NB_WORD_PADDED CLAMP(NB_WORD, WORD_PAR_PACKED)
// number of long long to store all the words.
#define NB_WORD_PACKED CLAMP(NB_WORD_PADDED / WORD_PAR_PACKED, 32 / SIZEOF_PACKED)

// values to restore old calc
#define IDX_CALC(idx) (idx / WORD_PAR_PACKED)
#define MASK_CALC(idx) (1LL << (idx % WORD_PAR_PACKED))

#define IS_AVAILABLE(available, idx) ((available[IDX_CALC(i)] & MASK_CALC(i)) > 0)

#define NB_COMBI (CLAMP(3 * 3 * 3 * 3 * 3, 32))

#define NB_TENTA 6
#define WORD_SIZE 5
#define N_BESTS 1

#define OCR_MAX_PARAMS 16

struct Save
{
    PACKED_TYPE *available;
    char *from_wordle;

    double bests[N_BESTS];
    int bests_idx[N_BESTS];

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
    int automat;
    int nb_thread;
    int disable;
    int limited;
    int prev;
    int rand;
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
int update_state(struct Save *data, const char *buffer, char *result);
int find_best(struct Save *status, int nb_thread);



#endif /* WORDLE_H */
