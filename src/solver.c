#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "wordle.h"

// data.c
extern const char *dataset[];
extern const char *used[];

// main.c
extern unsigned char mat[NB_WORD][NB_WORD];
extern int combis[NB_WORD][NB_COMBI];
extern struct Param params;

double scores(struct Save *data, size_t i);

void copy_save(struct Save* src, struct Save* dst)
{
    // printf("copy\n");
    memcpy(dst->available, src->available, sizeof(char) * NB_WORD);
    
    // memcpy(dst->from_wordle, src->from_wordle, sizeof(char) * NB_WORD);

    for (int i = 0; i < N_BESTS; i++) 
    {
        dst->bests[i] = 1000000.0;
        dst->bests_idx[i] = -1;
    }

    dst->turn = src->turn;
    dst->prev = src->prev;
    dst->prev_ans = src->prev_ans;
    // printf("copying values: prev %d, prev ans: %d\n", dst->prev, dst->prev_ans);
}

struct Save* clone_save(struct Save* value)
{
    // printf("clone\n");
    struct Save* ret = calloc(1, sizeof(struct Save));
    ret->available = calloc(NB_WORD, sizeof(char));
    
    // This data is never modified
    ret->from_wordle = value->from_wordle;

    copy_save(value, ret);

    return ret;
}

void free_save(struct Save* ptr)
{
    // printf("free\n");
    if (ptr)
    {
        free(ptr->available);
        // free(ptr->from_wordle);
        free(ptr);
    }
}

unsigned char result_to_char(const char *buffer) {
    unsigned char ret = 0;

    for (size_t i = 0; i < WORD_SIZE; i++) {
        ret *= 3;
        switch (buffer[i]) {
            case 'n':
                ret += 0;
                break;
            case 'm':
                ret += 1;
                break;
            case 'y':
                ret += 2;
                break;
        }
    }

    if (ret >= NB_COMBI) {
        printf("wtf: %s %d", buffer, ret);
    }

    return ret;
}

void char_to_result(char a, char ret[WORD_SIZE]) {

    for (size_t i = 0; i < WORD_SIZE; i++) {
        // printf("%hhd ", a % 3);
        switch (a % 3) {
            case 0:
                // printf("n | ");
                ret[WORD_SIZE - 1 - i] = 'n';
                break;
            case 1:
                // printf("m | ");
                ret[WORD_SIZE - 1 - i] = 'm';
                break;
            case 2:
                // printf("y | ");
                ret[WORD_SIZE - 1 - i] = 'y';
                break;
        }

        a /= 3;
    }

    // printf("\n");
}

int valid_hard(int prev, char prev_ans, int idx) {

    char answer[WORD_SIZE + 1] = { 0 }; 
    char_to_result(prev_ans, answer);

    // printf(" prev = %s, idx = %s, char_to_result: %s\n", dataset[prev], dataset[idx], answer);

    int data = 0;
    int req = 0;

    for (int i = 0; i < WORD_SIZE; i++) {
        // compact answer
        data |= compact(dataset[idx][i]);
        
        if (answer[i] == 'y' && dataset[idx][i] != dataset[prev][i])
            return 0;

        if (answer[i] == 'm')
            req |= compact(dataset[prev][i]);
    }

    return (data & req) == req;
}


unsigned char get_combi(const char *buffer, const char *expect) {
   // char result[WORD_SIZE + 1];
    
    // unsigned char ret = 0;

    // char tmp[WORD_SIZE + 1];
    char histo[26] = { 0 };
    
    unsigned char ret1 = 0;
    for (size_t i = 0; i < WORD_SIZE; i++) {
        ret1 *= 3;

        if (buffer[i] == expect[i]) {
            ret1 += 2;
        }
        else {
            histo[expect[i] - 'a']++;
        }

    }

    unsigned char ret2 = 0;
    for (int i = 0; i < WORD_SIZE; i++) {
        ret2 *= 3;

        if (buffer[i] != expect[i] && histo[buffer[i] - 'a']) {
            ret2 += 1;
            histo[buffer[i] - 'a']--;
        }
    }

    return ret1 + ret2; // result_to_char(result);
}

struct initData {
    size_t begin;
    size_t end;
    size_t j;
};

void *worker_init(void *arg)
{
    struct initData *data = (struct initData *)arg;

    for (size_t i = data->begin; i < data->end; i++)
    {
        for (size_t j = 0; j < NB_WORD; j++) {
            mat[j][i] = get_combi(dataset[i], dataset[j]);
        }
    }
}

void init_math(int nb_thread)
{
    struct initData *datas =
        (struct initData *)calloc(sizeof(struct initData), nb_thread);

    int repartition = NB_WORD / nb_thread;
    int a = 0;

    for (int i = 0; i < nb_thread; i++)
    {
        datas[i].begin = a;

        a += repartition;

        datas[i].end = a;
    }

    datas[nb_thread - 1].end = NB_WORD;

    pthread_t *threads = (pthread_t *)calloc(sizeof(pthread_t), nb_thread);

    for (int i = 0; i < nb_thread; i++)
    {
        pthread_create(&threads[i], NULL, worker_init, &datas[i]);
    }


    for (int i = 0; i < nb_thread; i++)
    {
        pthread_join(threads[i], NULL);
    }

    free(datas);
    free(threads);
}

void init(struct Save *data, char *buffer, char *result, struct Param params)
{
    for (int i = 0; i < NB_WORD; i++)
    {
        data->available[i] = 1;
    }


    size_t j = 0;
    for (size_t i = 0; i < NB_USED; i++)
    {
        int tmp = -1;

        while (tmp < 0 && j < NB_WORD)
        {
            tmp = strcmp(dataset[j], used[i]);
            data->from_wordle[j] = tmp == 0;
            j++;
        }
    }

    if (params.limited)
    {
        for (size_t i = 0; i < NB_WORD; i++)
        {
            data->available[i] = data->from_wordle[i];
        }
    }

    fprintf(stderr, "Starting matrix: ");
    init_math(params.nb_thread); 
    fprintf(stderr, "Done\n");

    fprintf(stderr, "Starting combis count: ");
    for (size_t i = 0; i < NB_WORD; i++) {
        for (size_t j = 0; j < NB_COMBI; j++) {
            combis[i][j] = 0;
        }

        for (size_t j = 0; j < NB_WORD; j++) {
            combis[i][mat[i][j]]++;
        }
    }
    fprintf(stderr, "Done\n");

    buffer[WORD_SIZE] = 0;
    result[WORD_SIZE] = 0;
}

void update_available(struct Save *data, const char *buffer, const char *result)
{
    size_t tmp = NB_WORD + 1;
    for (size_t i = 0; i < NB_WORD; i++) {
        if (!strcmp(buffer, dataset[i])) {
            tmp = i;
        }
    }

    assert(tmp < NB_WORD);

    const unsigned char combi = result_to_char(result);

    data->prev = tmp;
    data->prev_ans = combi;

    for (size_t i = 0; i < NB_WORD; i++)
    {
        data->available[i] &= combi == mat[i][tmp];
    }
}


void update_available2(struct Save *data, size_t idx, const unsigned char combi)
{
    for (size_t i = 0; i < NB_WORD; i++)
    {
        data->available[i] &= combi == mat[i][idx];
    }
}

int update_state(struct Save *data, const char *buffer, char *result)
{
    int ok = 0;

    int already_m = 0;

    for (int i = 0; i < WORD_SIZE; i++) {
        if (result[i] == 'y') {
            ok += 1;
        }
    }

    return ok;
}

int insert_best(int idx, double score, int idxs[N_BESTS], double scores[N_BESTS])
{
    int tmp_idx;
    double tmp_score;

    for (int i = 0; i < N_BESTS; i++) {
        if (scores[i] > score) {
            tmp_idx = idxs[i];
            tmp_score = scores[i];

            idxs[i] = idx;
            scores[i] = score;

            idx = tmp_idx;
            score = tmp_score;
        }
    }
}

int find_best_thread_less(struct Save *status, size_t start, size_t end) {
    double best_score = 10000000.0;
    // printf("1: %f\n", status->best_score);

    int idx = -1;

    // printf("0 ");
    for (size_t i = start; i < end; i++)
    {
        // if (end != NB_WORD)  printf("thread: %6zu/%6zu/%6zu %f\n", start, i, end, (float)(i - start) / (float)(end - start) * 100.f);
        double score = 0;

        if (!params.hard || valid_hard(status->prev, status->prev_ans, i)) {
            score = scores(status, i);
            // printf("score = %f \n", score);
        }
        else {
            score = 0;
        }
        // printf("%s = %f\n", dataset[i], score);
        insert_best(i, score, status->bests_idx, status->bests);
    }

    // printf("%ld, %ld => %d, %f\n", start, end, idx, status->best_score);

    return status->bests_idx[0];
}

void copy_update_avai(struct Save* src, struct Save* dst, size_t idx, const unsigned char combi) {
    for (int i = 0; i < N_BESTS; i++) 
    {
        dst->bests[i] = 1000000.0;
        dst->bests_idx[i] = -1;
    }

    for (size_t i = 0; i < NB_WORD; i++)
    {
        dst->available[i] = src->available[i] & combi == mat[i][idx];
    }
}

double scores(struct Save *data, size_t i)
{
    int rep[NB_COMBI];

    for (size_t j = 0; j < NB_COMBI; j++) {
        rep[j] = 0;
    }

    for (size_t j = 0; j < NB_WORD; j++) {
        rep[mat[i][j]] += data->available[j];
    }

    // printf("] ");
    double sum = 0;

    double E = 0.;

    for (size_t i = 0; i < NB_COMBI; i++)
    {
        sum += rep[i];
    }

    struct Save* rec_save = clone_save(data);
    rec_save->turn += 1;

    double p = 0;
    for (size_t j = 0; j < NB_COMBI; j++) {
        if (rep[j] != 0) {
            p = rep[j] / sum;

            if (data->turn < NB_TENTA - NB_TENTA) {
                // printf("depth: %d, combi: %d,  word: %s, \n", data->turn, j, dataset[i]);

                copy_update_avai(data, rec_save, i, j);

                // copy_save(data, rec_save);
                // rec_save->turn += 1;
                // update_available2(rec_save, i, (const unsigned char)j);

                int best_idx = find_best_thread_less(rec_save, 0, NB_WORD);

                // moins car le score sera negativé apres
                E -= p * rec_save->bests[0];

            }


            E -= p * log2(p);
            // printf("p = %f => E = %f\n", p, E);
        }
    }

    free_save(rec_save);

    return -E;
}

void *worker(void *arg)
{
    struct workData *data = (struct workData *)arg;

    data->idx = find_best_thread_less(data->data, data->begin, data->end);
    data->score = data->data->bests[0];
}

int find_best(struct Save *status, int nb_thread)
{
    struct workData *datas =
        (struct workData *)calloc(sizeof(struct workData), nb_thread);

    int repartition = NB_WORD / nb_thread;
    int a = 0;

    for (int i = 0; i < nb_thread; i++)
    {
        datas[i].begin = a;

        a += repartition;

        datas[i].end = a;
        datas[i].data = clone_save(status);
    }

    datas[nb_thread - 1].end = NB_WORD;

    pthread_t *threads = (pthread_t *)calloc(sizeof(pthread_t), nb_thread);

    for (int i = 0; i < nb_thread; i++)
    {
        pthread_create(&threads[i], NULL, worker, &datas[i]);
    }

    double best_score = 10000000.0;
    int idx = -1;

    for (int i = 0; i < nb_thread; i++)
    {
        pthread_join(threads[i], NULL);

        // printf("score thread %d: %f\n", i, datas[i].score);

        // for (size_t j = 0; j < N_BESTS; j++) {
        //     printf("%s: %f\n", dataset[datas[i].data->bests_idx[j]], datas[i].data->bests[j]);
        // }
        // printf("\n");

        // printf("%s: %f\n\n", dataset[datas[i].idx], datas[i].score);
        if (datas[i].score < best_score)
        {
            best_score = datas[i].score;
            idx = datas[i].idx;
        }
        free_save(datas[i].data);
    }

    free(datas);
    free(threads);

    return idx;
}

/*
find_best(save):
    for each word(word):
        - compute his score
        - updated_save = select it(word)
            - find best again
*/





