#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "wordle.h"

#define NB_COMBI (3 * 3 * 3 * 3 * 3)
#define NB_TENTA 6

struct Param
{
    const char *real_word;
    int automat;
    int nb_thread;
    int disable;
    int limited;
    int prev;
    int rand;
};

extern const char *dataset[];
extern const char *used[];
unsigned char mat[NB_WORD][NB_WORD];
int combis[NB_WORD][NB_COMBI];

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

        data->word_data[i] = 0;
        for (int j = 0; j < WORD_SIZE; j++)
        {
            data->word_data[i] |= compact(dataset[i][j]);
        }
    }

    for (int i = 0; i < WORD_SIZE; i++)
    {
        data->not_in[i] = 0;
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

void print_valid(struct Save *data)
{
    for (int i = 0; i < NB_WORD; i++)
    {
        if (data->available[i])
        {
            if (data->from_wordle[i])
            {
                printf("[*] ");
            }
            else
            {
                printf("[ ] ");
            }

            printf("%s : scores %f\n", dataset[i], data->scores[i]);
        }
    }
}

struct Counter
{
    int total;
    int wordle;
};

struct Counter count_remaining(struct Save *data)
{
    struct Counter count = { 0 };
    for (int i = 0; i < NB_WORD; i++)
    {
        if (data->available[i])
        {
            if (data->from_wordle[i])
            {
                count.wordle++;
            }
            count.total++;
        }
    }

    return count;
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

int update_state(struct Save *data, const char *buffer, char *result)
{
    int ok = 0;

    int already_m = 0;

    for (int i = 0; i < WORD_SIZE; i++) {
        if (result[i] == 'y') {
            data->know[i] = buffer[i];
            ok += 1;
        }
    }

    return ok;

    // todo

    for (int i = 0; i < WORD_SIZE; i++)
    {
        switch (result[i])
        {
        case 'y':
            break;

        case 'n':
            if (!(already_m & compact(buffer[i])))
            {
                for (int j = 0; j < WORD_SIZE; j++)
                {
                    data->not_in[j] |= compact(buffer[i]);
                }
            }
            else
            {
                data->not_in[i] |= compact(buffer[i]);
            }

            break;

        case 'm':
            data->misplaced |= compact(buffer[i]);
            already_m |= compact(buffer[i]);
            data->not_in[i] |= compact(buffer[i]);
            break;

        default:
            printf("You fucking moron that not a valid letter\n");
            break;
        }
    }

    return ok;
}

struct Score scores(struct Save *data, size_t i)
{
    double rep[NB_COMBI];
    // char result[WORD_SIZE + 1];
    // result[WORD_SIZE] = 0;

    // struct Save new_data = { data->available, data->from_wordle,
    //                          data->word_data, data->scores, 0 };

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

    double p = 0;
    for (size_t i = 0; i < NB_COMBI; i++) {
        if (rep[i] != 0) {
            p = rep[i] / sum;
            
            E += p * log2(1. / p);
            // printf("p = %f => E = %f\n", p, E);
        }
    }

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

    double best_score = 10000000.0;
    int idx = -1;

    for (size_t i = data->begin; i < data->end; i++)
    {
        struct Score score_all = scores(data->data, i);

        double score = score_all.E;
        data->data->scores[i] = score;
        //printf("%f\n", score);
        if (score < best_score)
        {
            best_score = score;
            idx = i;
        }
    }

    data->idx = idx;
    data->score = best_score;
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
        datas[i].data = status;
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

        if (datas[i].score < best_score)
        {
            best_score = datas[i].score;
            idx = datas[i].idx;
        }
    }

    free(datas);
    free(threads);

    return idx;
}

#include <stdlib.h>
#include <string.h>

struct Param parse_arg(int argc, char **argv)
{
    struct Param ret = { 0 };

    ret.nb_thread = 1;

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-a"))
        {
            ret.automat = 1;
        }
        else if (!strcmp(argv[i], "-w"))
        {
            if (i + 1 < argc)
            {
                ret.real_word = argv[i + 1];
                i++;

                if (strlen(ret.real_word) != WORD_SIZE)
                {
                    fprintf(stderr, "Only word of %d letters are allowed",
                            WORD_SIZE);
                    exit(1);
                }
            }
            else
            {
                fprintf(stderr, "-w used but no given word");
                exit(1);
            }
        }
        else if (!strcmp(argv[i], "-t"))
        {
            if (i + 1 < argc)
            {
                ret.nb_thread = atoi(argv[i + 1]);
                i++;

                if (ret.nb_thread == -1)
                {
                    fprintf(stderr, "Invalid number of thread");
                    exit(1);
                }
            }
        }
        else if (!strcmp(argv[i], "-d"))
        {
            ret.disable = 1;
        }
        else if (!strcmp(argv[i], "--limited"))
        {
            ret.limited = 1;
        }
        else if (!strcmp(argv[i], "-p")) 
        {
            ret.prev = 1;
        }
        else if (!strcmp(argv[i], "-r")) {
            ret.rand = 1;
        }
        else
        {
            fprintf(stderr, "Unknown argument, '%s'\n", argv[i]);
            exit(1);
        }
    }

    return ret;
}

int main(int argc, char **argv)
{
    printf("Hello world: %zu words possible\n", NB_WORD);

    const char *prev[NB_TENTA][WORD_SIZE];

    struct Param params = parse_arg(argc, argv);

    int available[NB_WORD];
    int word_data[NB_WORD];
    int from_wordle[NB_WORD];
    double scores[NB_WORD];

    struct Save data = { available, from_wordle, word_data, scores, 0 };

    char buffer[WORD_SIZE + 1];
    char result[WORD_SIZE + 1];

    init(&data, buffer, result, params);

    if (params.prev) {
        printf("Please enter other's sequences\n");
        printf("Press enter 2 times to change sequence and 3 times to stop\n");

        unsigned char combi[NB_COMBI];

        unsigned char buffer[NB_WORD];
   
        char c = '\0';

        int count = 0;
        while (1) {
            count = 0;
            for (size_t i = 0; i < NB_COMBI; i++) {
                combi[i] = 0;
            }

            while (1) {
                c = getchar();
                
                if (c == '\n') {
                    printf("break 2\n");
                    break;
                }

                buffer[0] = c;
                buffer[1] = getchar();
                buffer[2] = getchar();
                buffer[3] = getchar();
                buffer[4] = getchar();

                getchar();

                combi[result_to_char(buffer)]++;
                count++;
            }

            if (count == 0) {
                printf("break 1\n");
                break;
            }

            int count2 = 0;
            for (size_t i = 0; i < NB_WORD; i++) {
                for (size_t j = 0; j < NB_COMBI; j++) {
                    if (data.available[i] && combis[i][j] < combi[j]) {
                        // printf("removing word: %s\n" , dataset[i]);
                        data.available[i] = 0;
                        count2++;
                    }
                }
            }

            printf("removed %d word(s)\n", count2);

        }

        printf("End of prev\n");
    }

    if (params.rand) {
        srand(time(NULL));

        int rand_val = rand() % NB_USED;
        // printf("%d < %ld\n", rand_val, NB_USED);
        params.real_word = used[rand_val];
        printf("Word choosen: %s\n", params.real_word);
    }

    int ok = 0;
    while (ok != WORD_SIZE && data.turn < NB_TENTA)
    {
        ok = 0;

        struct Counter remaining = count_remaining(&data);
        int conseil = 0;

        if (!params.disable && remaining.total > 5 && remaining.wordle > 2)
        {
            conseil = find_best(&data, params.nb_thread);
            printf("Select a word, %d remainings, %d from wordle (recommanded "
                   "= %s):\n",
                   remaining.total, remaining.wordle, dataset[conseil]);
        }
        else if (!params.disable && remaining.total > 2)
        {
            conseil = find_best(&data, params.nb_thread);
            print_valid(&data);
            printf("Select a word, %d remainings, %d from wordle (recommanded "
                   "= %s):\n",
                   remaining.total, remaining.wordle, dataset[conseil]);
        }
        else
        {
            print_valid(&data);
            printf("Select a word:\n");
        }

        if (params.automat && remaining.total > 2)
        {
            for (int i = 0; i < WORD_SIZE; i++)
            {
                buffer[i] = dataset[conseil][i];
            }
        }
        else
        {
            for (int i = 0; i < WORD_SIZE; i++)
            {
                buffer[i] = getchar();
            }

            getchar();
        }

        if (params.real_word)
        {
            auto_fill(buffer, params.real_word, result);
            printf("Autofilled with : %s\n", result);
        }
        else
        {
            printf(
                "Result obtain(n = not in word, m = misplaced, y = good):\n");
            for (int i = 0; i < WORD_SIZE; i++)
            {
                result[i] = getchar();
            }

            getchar();
        }

        // printf("buffer %s\n", buffer);
        // printf("result %s\n", result);

        ok = update_state(&data, buffer, result);

        update_available(&data, buffer, result);

        data.turn += 1;

        printf("\n");
    }

    if (ok == WORD_SIZE)
        printf("Weldone you found it! En %d tenta.\n", data.turn);
    else
        printf("Sad ça passe pas\n");

    return 0;
}
