#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wordle.h"

#define NB_COMBI (3 * 3 * 3 * 3 * 3)
#define NB_TENTA 6

struct Param
{
    char *real_word;
    int automat;
    int nb_thread;
    int disable;
    int limited;
};

extern const char *dataset[];
extern const char *used[];

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

int valid_word(int word, struct Save *data)
{
    if (!data->available[word]) {
        return 0; 
    }

    for (int i = 0; i < WORD_SIZE; i++)
    {
        int tmp = compact(dataset[word][i]);

        if ( data->know[i] != dataset[word][i] &&
            (data->know[i] != 0 || data->not_in[i] & tmp))
        {
            // printf("invalid 1(%d): %s = %d | %d | %d || %d & %d \n",
            //        i,
            //        dataset[word], 
            //        data->know[i] != 0,
            //        data->know[i] != dataset[word][i],
            //        data->not_in[i] & tmp,
            //        data->not_in[i],
            //        tmp
            //    );

            // for (char j = 'a'; j < 'z'; j++) {
            // 
            //    if (data->not_in[i] & compact(j)) {
            //        printf("%c", j);
            //    }
            //}
            //printf("\n");

            return 0;
        }
    }

    if ((data->word_data[word] & data->misplaced) != data->misplaced)
    {
        // printf("invalid 2: %s\n", dataset[word]);
        return 0;
    }

    return 1;
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

    for (int i = 0; i < WORD_SIZE; i++) {
        if (result[i] == 'y') {
            data->know[i] = buffer[i];
            ok += 1;
        }
    }

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

const char possibilite[] = { 'y', 'n', 'm' };

char *possibilites[] = {
    "yyyyy", "nyyyy", "myyyy", "ynyyy", "nnyyy", "mnyyy", "ymyyy", "nmyyy",
    "mmyyy", "yynyy", "nynyy", "mynyy", "ynnyy", "nnnyy", "mnnyy", "ymnyy",
    "nmnyy", "mmnyy", "yymyy", "nymyy", "mymyy", "ynmyy", "nnmyy", "mnmyy",
    "ymmyy", "nmmyy", "mmmyy", "yyyny", "nyyny", "myyny", "ynyny", "nnyny",
    "mnyny", "ymyny", "nmyny", "mmyny", "yynny", "nynny", "mynny", "ynnny",
    "nnnny", "mnnny", "ymnny", "nmnny", "mmnny", "yymny", "nymny", "mymny",
    "ynmny", "nnmny", "mnmny", "ymmny", "nmmny", "mmmny", "yyymy", "nyymy",
    "myymy", "ynymy", "nnymy", "mnymy", "ymymy", "nmymy", "mmymy", "yynmy",
    "nynmy", "mynmy", "ynnmy", "nnnmy", "mnnmy", "ymnmy", "nmnmy", "mmnmy",
    "yymmy", "nymmy", "mymmy", "ynmmy", "nnmmy", "mnmmy", "ymmmy", "nmmmy",
    "mmmmy", "yyyyn", "nyyyn", "myyyn", "ynyyn", "nnyyn", "mnyyn", "ymyyn",
    "nmyyn", "mmyyn", "yynyn", "nynyn", "mynyn", "ynnyn", "nnnyn", "mnnyn",
    "ymnyn", "nmnyn", "mmnyn", "yymyn", "nymyn", "mymyn", "ynmyn", "nnmyn",
    "mnmyn", "ymmyn", "nmmyn", "mmmyn", "yyynn", "nyynn", "myynn", "ynynn",
    "nnynn", "mnynn", "ymynn", "nmynn", "mmynn", "yynnn", "nynnn", "mynnn",
    "ynnnn", "nnnnn", "mnnnn", "ymnnn", "nmnnn", "mmnnn", "yymnn", "nymnn",
    "mymnn", "ynmnn", "nnmnn", "mnmnn", "ymmnn", "nmmnn", "mmmnn", "yyymn",
    "nyymn", "myymn", "ynymn", "nnymn", "mnymn", "ymymn", "nmymn", "mmymn",
    "yynmn", "nynmn", "mynmn", "ynnmn", "nnnmn", "mnnmn", "ymnmn", "nmnmn",
    "mmnmn", "yymmn", "nymmn", "mymmn", "ynmmn", "nnmmn", "mnmmn", "ymmmn",
    "nmmmn", "mmmmn", "yyyym", "nyyym", "myyym", "ynyym", "nnyym", "mnyym",
    "ymyym", "nmyym", "mmyym", "yynym", "nynym", "mynym", "ynnym", "nnnym",
    "mnnym", "ymnym", "nmnym", "mmnym", "yymym", "nymym", "mymym", "ynmym",
    "nnmym", "mnmym", "ymmym", "nmmym", "mmmym", "yyynm", "nyynm", "myynm",
    "ynynm", "nnynm", "mnynm", "ymynm", "nmynm", "mmynm", "yynnm", "nynnm",
    "mynnm", "ynnnm", "nnnnm", "mnnnm", "ymnnm", "nmnnm", "mmnnm", "yymnm",
    "nymnm", "mymnm", "ynmnm", "nnmnm", "mnmnm", "ymmnm", "nmmnm", "mmmnm",
    "yyymm", "nyymm", "myymm", "ynymm", "nnymm", "mnymm", "ymymm", "nmymm",
    "mmymm", "yynmm", "nynmm", "mynmm", "ynnmm", "nnnmm", "mnnmm", "ymnmm",
    "nmnmm", "mmnmm", "yymmm", "nymmm", "mymmm", "ynmmm", "nnmmm", "mnmmm",
    "ymmmm", "nmmmm", "mmmmm"
};

struct Score scores(struct Save *data, const char *buffer)
{
    double rep[NB_COMBI];
    char result[WORD_SIZE + 1];
    result[WORD_SIZE] = 0;

    struct Save new_data = { data->available, data->from_wordle,
                             data->word_data, data->scores, 0 };

    for (int j = 0; j < NB_COMBI; j++)
    {
        new_data.available = data->available;
        for (int k = 0; k < WORD_SIZE; k++)
        {
            new_data.know[k] = data->know[k];
            new_data.not_in[k] = data->not_in[k];
        }
        new_data.misplaced = data->misplaced;

        // printf("type: %s ", result);

        update_state(&new_data, buffer, possibilites[j]);

        rep[j] = 0;
        for (int i = 0; i < NB_WORD; i++)
        {
            if (data->available[i] && (valid_word(i, &new_data)))
            {
                rep[j] += 1;
            }
        }

        // printf("%f ", rep[j]);
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
    // printf("] ");
    double sum = 0;
    for (int i = 0; i < NB_COMBI; i++)
    {
        sum += rep[i];
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

    return ret;
}

void *worker(void *arg)
{
    struct workData *data = (struct workData *)arg;

    double best_score = 10000000.0;
    int idx = -1;

    for (int i = data->begin; i < data->end; i++)
    {
        struct Score score_all = scores(data->data, dataset[i]);

        double score = score_all.std;
        data->data->scores[i] = score;
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

    int idx_guess = 0;

    for (int i = 0; i < NB_WORD; i++) {
        if (!strcmp(dataset[i], "guess")) {
            idx_guess = i;
            break;
        }
    }

    printf("%d: %s", idx_guess, dataset[idx_guess]);

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
