#include "corpus.h"
#include "logger/logger.h"
#include <stdlib.h>

struct square_corpus *make_square_corpus(struct square *first) {
    struct square_corpus *sc = calloc(1, sizeof(struct square_corpus));

    if (!sc) {
        fatal("OOm while creating Square corpus");
        exit(1);
    }

    sc->index = 0;

    if (first) {
        sc->corpus[0][0] = first;
        sc->index++;
    }

    return sc;
}

void free_square_corpus(struct square_corpus *sc) {
    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 5; x++) {
            free_square(sc->corpus[y][x]);
        }
    }

    free(sc);
}