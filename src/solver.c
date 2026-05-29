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

struct Score scores(struct Save *data, size_t i);

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
            case 'y':
                ret += 0;
                break;
            case 'm':
                ret += 1;
                break;
            case 'n':
                ret += 2;
                break;
        }
    }

    if (ret >= NB_COMBI) {
        printf("wtf: %s %d", buffer, ret);
    }

    return ret;
}


unsigned char get_combi(const char *buffer, const char *expect) {
    char result[WORD_SIZE + 1];
    
    char tmp[WORD_SIZE + 1];
    for (size_t i = 0; i < WORD_SIZE; i++) {
        tmp[i] = expect[i];
    }
    
    for (int i = 0; i < WORD_SIZE; i++) {
        if (buffer[i] == tmp[i]) {
            result[i] = 'y';
        }
        else {
            int find = 0;
            for (int j = 0; j < WORD_SIZE; j++) {
                if (i != j && tmp[j] == buffer[i] && buffer[j] != tmp[j]) {
                    find += 1;
                    tmp[j] = 0;
                    break;
                }
            }

            result[i] = find ? 'm' : 'n';
        }
    }


    return result_to_char(result);
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
            mat[i][j] = get_combi(dataset[i], dataset[j]);
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
            // printf("%s | %s\n", dataset[j], used[i]);
            tmp = strcmp(dataset[j], used[i]);
            data->from_wordle[j] = tmp == 0;
            j++;
        }

        // if (tmp == 0) {
        //     printf("-> %s | %s\n", dataset[j], used[i]);
        //     data->from_wordle[j - 1] = 1;
        // }
    }

    if (params.limited)
    {
        for (size_t i = 0; i < NB_WORD; i++)
        {
            data->available[i] = data->from_wordle[i];
        }
    }

    printf("Starting matrix: ");
    init_math(params.nb_thread); 
    printf("Done\n");

    printf("Starting combis count: ");
    for (size_t i = 0; i < NB_WORD; i++) {
        for (size_t j = 0; j < NB_COMBI; j++) {
            combis[i][j] = 0;
        }

        for (size_t j = 0; j < NB_WORD; j++) {
            combis[i][mat[j][i]]++;
        }
    }
    printf("Done\n");

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

    for (size_t i = 0; i < NB_WORD; i++)
    {
        data->available[i] &= combi == mat[tmp][i];
    }
}


void update_available2(struct Save *data, size_t idx, const unsigned char combi)
{
    for (size_t i = 0; i < NB_WORD; i++)
    {
        data->available[i] &= combi == mat[idx][i];
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
        // if (params.hard > 0 && !valid_hard()) {
        // TODO 
        // }

        struct Score score_all = scores(status, i);

        double score = score_all.E;
        
        // printf("%s = %f\n", dataset[i], score);
        insert_best(i, score, status->bests_idx, status->bests);
    }

    // printf("%ld, %ld => %d, %f\n", start, end, idx, status->best_score);

    return status->bests_idx[0];
}

struct Score scores(struct Save *data, size_t i)
{
    double rep[NB_COMBI];

    for (size_t j = 0; j < NB_COMBI; j++) {
        rep[j] = 0;
    }

    for (size_t j = 0; j < NB_WORD; j++) {
        if (mat[j][i] >= NB_COMBI) {
            printf("Something fuck up : %d\n", mat[j][i]);
        }
        else {
            if (data->available[j]) {
                rep[mat[j][i]] += 1;
            }
        }
    }

    // printf("] ");
    double sum = 0;

    double E = 0.;

    for (size_t i = 0; i < NB_COMBI; i++)
    {
        sum += rep[i];
    }

    struct Save* rec_save = clone_save(data);

    double p = 0;
    for (size_t j = 0; j < NB_COMBI; j++) {
        if (rep[j] != 0) {
            p = rep[j] / sum;

            if (data->turn < NB_TENTA - NB_TENTA) {
                printf("depth: %d, combi: %d,  word: %s, \n", data->turn, j, dataset[i]);

                copy_save(data, rec_save);
                rec_save->turn += 1;
                update_available2(rec_save, i, (const unsigned char)j);

                int best_idx = find_best_thread_less(rec_save, 0, NB_WORD);

                // moins car le score sera negativé apres
                E -= p * rec_save->bests[0];

            }


            E += p * log2(1. / p);
            // printf("p = %f => E = %f\n", p, E);
        }
    }

    free_save(rec_save);

    double mean = sum / NB_COMBI;
    // printf("sum: %f, mean: %f ", sum, mean);

    sum = 0;

    double max = -1;

    for (int i = 0; i < NB_COMBI; i++)
    {
        double tmp = rep[i] - mean;
        sum += tmp * tmp;

        if (max < rep[i])
        {
            max = rep[i];
        }
    }

    // printf("\n");

    // exit(42);

    struct Score ret;

    ret.mean = mean;
    ret.std = sum / NB_COMBI;
    ret.max = max;
    ret.E = - E;

    return ret;
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





