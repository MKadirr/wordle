#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include <stdlib.h>

#include "wordle.h"

#define NB_TENTA 6

extern const char *dataset[];
extern const char *used[];
unsigned char mat[NB_WORD][NB_WORD];
int combis[NB_WORD][NB_COMBI];

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


struct Param params;

int main(int argc, char **argv)
{
    printf("Hello world: %zu words possible\n", NB_WORD);

    const char *prev[NB_TENTA][WORD_SIZE];

    params = parse_arg(argc, argv);

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
