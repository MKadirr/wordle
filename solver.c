#include "wordle.h"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stddef.h>

#define NB_COMBI (3 * 3 * 3 * 3 * 3)
#define NB_TENTA 6

extern const char* dataset[];
extern const char** used[];

void init(struct Save *data, char *buffer, char *result)
{
    for (int i = 0; i < NB_WORD; i++)
    {
        data->available[i] = 1;

        data->word_data[i] = 0;
        for (int j = 0; j < WORD_SIZE; j++) {
            data->word_data[i] |= compact(dataset[i][j]);
        }
    }

    for (int i = 0; i < WORD_SIZE; i++) {
        data->not_in[i] = 0;
    }

    buffer[WORD_SIZE] = 0;
    result[WORD_SIZE] = 0;
}

void print_valid(struct Save *data)
{
    for (int i = 0; i < NB_WORD; i++)
    {
        if (data->available[i])
        {
            printf("%s\n", dataset[i]);
        }
    }
}

int count_remaining(struct Save* data) {
    int count = 0;
    for (int i = 0; i < NB_WORD; i++)
    {
        if (data->available[i])
        {
            count++;
        }
    }

    return count;
}

int valid_word(int word, struct Save *data)
{
    int valid = 1;

    for (int i = 0; i < WORD_SIZE; i++)
    {
        int tmp = compact(dataset[word][i]);

        if ((data->know[i] != 0 && data->know[i] != dataset[word][i]) || data->not_in[i] & tmp)
        {
            valid = 0;
        }
    }

    if ((data->word_data[word] & data->misplaced) != data->misplaced)
    {
        valid = 0;
    }

    return valid;
} // eutar

void update_available(struct Save *data)
{
    int valid = 0;
    for (int i = 0; i < NB_WORD; i++)
    {
        data->available[i] &= valid_word(i, data);
    }
}

int update_state(struct Save *data, const char *buffer, char *result)
{
    int ok = 0;

    int already_m = 0;

    for (int i = 0; i < WORD_SIZE; i++)
    {
        switch (result[i])
        {
        case 'y':
            data->know[i] = buffer[i];
            ok += 1;
            break;

        case 'n':
            if (!(already_m & compact(buffer[i]))) {
                for (int j = 0; j < WORD_SIZE; j++) {
                    data->not_in[j] |= compact(buffer[i]);
                }
            }
            else {
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

const char possibilite[] = { 'y', 'n', 'm' };

struct Score scores(struct Save *data, const char *buffer)
{
    char result[WORD_SIZE + 1];
    result[WORD_SIZE] = 0;
    double rep[NB_COMBI];

    struct Save new_data = { data->available, data->word_data };

    for (int j = 0; j < NB_COMBI; j++)
    {
        int a = j % 3;
        int b = (j / (3)) % 3;
        int c = (j / (3 * 3)) % 3;
        int d = (j / (3 * 3 * 3)) % 3;
        int e = (j / (3 * 3 * 3 * 3)) % 3;

        new_data.available = data->available;
        for (int k = 0; k < WORD_SIZE; k++)
        {
            new_data.know[k] = data->know[k];
            new_data.not_in[k] = data->not_in[k];
        }
        new_data.misplaced = data->misplaced;

        result[0] = possibilite[a];
        result[1] = possibilite[b];
        result[2] = possibilite[c];
        result[3] = possibilite[d];
        result[4] = possibilite[e];

        //printf("type: %s ", result);

        update_state(&new_data, buffer, result);

        rep[j] = 0;
        for (int i = 0; i < NB_WORD; i++) {
            if (data->available[i] && (valid_word(i, &new_data))) {
                rep[j] += 1;
            }
        }

        //printf("%f ", rep[j]);
    }
    /*
    
    clean
    taper
    shame
    quake

    waste
    crane
    coate
    plage
    
    quake
    slurp
    nnmn
*/
    //printf("] ");
    double sum = 0;
    for (int i = 0; i < NB_COMBI; i++) {
        sum += rep[i];
    }

    double mean = sum / NB_COMBI;
    //printf("sum: %f, mean: %f ", sum, mean);

    sum = 0;

    double max = -1;

    for (int i = 0; i < NB_COMBI; i++) {
        double tmp = rep[i] - mean;
        sum += tmp * tmp;

        if (max < rep[i]) {
            max = rep[i];
        }
    }

    //printf("\n");

    //exit(42);

    struct Score ret;

    ret.mean = mean;
    ret.std = sum / NB_COMBI;
    ret.max = max;

    return ret;
}

void* worker(void* arg) {

    struct workData* data = (struct workData*) arg;

    double best_score = 10000000.0;
    int idx = -1;

    for (int i = data->begin; i < data->end; i++)
    {
        struct Score score_all = scores(data->data, dataset[i]);

        double score = score_all.std;
        // printf("%f\n", score);
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
    struct workData* datas = (struct workData*) calloc(sizeof(struct workData), nb_thread);

    int repartition = NB_WORD / nb_thread;
    int  a = 0;

    for (int i = 0; i < nb_thread; i++) {
        datas[i].begin = a;

        a += repartition;

        datas[i].end = a;
        datas[i].data = status;
    }

    datas[nb_thread - 1].end = NB_WORD;

    pthread_t* threads = (pthread_t*) calloc(sizeof(pthread_t), nb_thread);

    for (int i = 0; i < nb_thread; i++) {
        pthread_create(&threads[i], NULL, worker, &datas[i]);
    }

    double best_score = 10000000.0;
    int idx = -1;

    for (int i = 0; i < nb_thread; i++) {
        pthread_join(threads[i], NULL);

        if (datas[i].score < best_score) {
            best_score = datas[i].score;
            idx = datas[i].idx;
        }
    }

    free(datas);
    free(threads);

    return idx;
}

#include <string.h>
#include <stdlib.h>

struct Param {
    char* real_word;
    int automat;
    int nb_thread;
    int disable;
};

struct Param parse_arg(int argc, char** argv) {
    struct Param ret = { 0 } ;
    
    ret.nb_thread = 1;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-a")) {
            ret.automat = 1;
        }
        else if (!strcmp(argv[i], "-w")) {
            if (i + 1 < argc) {
                ret.real_word = argv[i + 1];
                i++;

                if (strlen(ret.real_word) != WORD_SIZE) {
                    fprintf(stderr, "Only word of %d letters are allowed", WORD_SIZE);
                    exit(1);
                }
            }
            else {
                fprintf(stderr, "-w used but no given word");
                exit(1);
            }
        }
        else if (!strcmp(argv[i], "-t")) {
            if (i + 1 < argc) {
                ret.nb_thread = atoi(argv[i + 1]);
                i++;

                if (ret.nb_thread == -1) {
                    fprintf(stderr, "Invalid number of thread");
                    exit(1);
                }
            }
        }
        else if (!strcmp(argv[i], "-d")) {
            ret.disable = 1;
        }
        else {
            fprintf(stderr, "Unknown argument, '%s'", argv[i]);
            exit(1);
        }
    }

    return ret;
}

int main(int argc, char **argv)
{
    printf("Hello world: %zu words possible\n", NB_WORD);

    const char* prev[NB_TENTA][WORD_SIZE];

    struct Param params = parse_arg(argc, argv);

    int available[NB_WORD];
    int word_data[NB_WORD];

    struct Save data = { available, word_data, 0 };

    char buffer[WORD_SIZE + 1];
    char result[WORD_SIZE + 1];

    init(&data, buffer, result);

    int ok = 0;
    while (ok != WORD_SIZE && data.turn < NB_TENTA)
    {
        ok = 0;

        int remaining = count_remaining(&data);
        int conseil = 0;

        if (!params.disable && remaining > 5) {
            conseil = find_best(&data, params.nb_thread);
            printf("Select a word, %d remainings (recommanded = %s):\n", remaining, dataset[conseil]);
        }
        else if (!params.disable && remaining > 2) {
            conseil = find_best(&data, params.nb_thread);
            print_valid(&data);
            printf("Select a word, %d remainings (recommanded = %s):\n", remaining, dataset[conseil]);
        }
        else {
            print_valid(&data);
            printf("Select a word:\n");
        }

        if (params.automat && remaining > 2) {
            for (int i = 0; i < WORD_SIZE; i++) {
                buffer[i] =  dataset[conseil][i];
            }
        }
        else {
            for (int i = 0; i < WORD_SIZE; i++)
            {
                buffer[i] = getchar();
            }

            getchar();
        }
        

        if (params.real_word) {
            auto_fill(buffer, params.real_word, result);
            printf("Autofilled with : %s\n", result);
        }
        else {
            printf("Result obtain(n = not in word, m = misplaced, y = good):\n");
            for (int i = 0; i < WORD_SIZE; i++)
            {
                result[i] = getchar();
            }

            getchar();
        }

        

        // printf("buffer %s\n", buffer);
        // printf("result %s\n", result);

        ok = update_state(&data, buffer, result);

        update_available(&data);
        
        data.turn += 1;

        printf("\n");
    }

    if (ok == WORD_SIZE)
        printf("Weldone you found it!\n");
    else
        printf("Sad ça passe pas\n");

    

    return 0;
}