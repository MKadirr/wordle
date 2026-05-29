#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "wordle.h"

void print_help_usage() {
    printf("usage: ./solver [OPTIONS]\n");

    

    printf("OPTIONS:\n");
    printf("    -a run the programme automatically by using the optimal word (dont work very well)\n");

    printf("    -w WORD       run the programme and autocomplete he wordle answer\n");
    printf("    -t NB_THREAD  use the given number of thread to compute heavy calculation\n");
    printf("    -d            disable all computation, simple game of wordle\n");
    printf("    --bench       run the benchmark code: not ready yet\n");
    printf("    --hard        specify the hard deficulty to compute optimal word: NOT IMPLEMENTED\n");
    printf("    --help        display that programme\n");
    printf("    --limited     run the programme with only wordle used word: some of in your wordle game may not be in that list\n");
    printf("    -p            enable the filtering using other grid to reduce possibilities\n");
    printf("    -r            choose a random word for that game\n");
    printf("    -I PATH       path to discord's wordle's board images\n");

}

struct Param parse_arg(int argc, char **argv)
{
    struct Param ret = { 0 };

    ret.imgs = ctr_vector();

    ret.nb_thread = 1;

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-a"))
        {
            ret.automat = 1;
        }
        else if (!strcmp(argv[i], "-I")) {

            if (i + 1 < argc)
            {
                i++;

                push_vector(ret.imgs, argv[i]);
            }
            else
            {
                fprintf(stderr, "-I used but no path given");
                exit(1);
            }

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
        else if (!strcmp(argv[i], "--bench"))
        {
            ret.bench = 1;
        }
        else if (!strcmp(argv[i], "--hard"))
        {
            ret.hard = 1;
        }
        else if (!strcmp(argv[i], "--limited"))
        {
            ret.limited = 1;
        } 
        else if (!strcmp(argv[i], "--help"))
        {
            print_help_usage();
            exit(0);
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
