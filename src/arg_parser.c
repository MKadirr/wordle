#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "wordle.h"


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
