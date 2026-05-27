#include "corpus.h"
#include "logger/logger.h"
#include <stdlib.h>
#include <stdbool.h>

struct square_corpus *make_square_corpus(struct square *first) {
    struct square_corpus *sc = calloc(1, sizeof(struct square_corpus));

    if (!sc) {
        fatal("OOm while creating Square corpus");
        exit(1);
    }

    sc->index = 0;
    sc->offset = 0;

    if (first) {
        sc->corpus[0] = first;
        sc->index++;
    }

    return sc;
}

void free_square_corpus(struct square_corpus *sc) {
    for (int i = 0; i < sc->index; i++) {
        free_square(sc->corpus[i]);
    }

    free(sc);
}

bool add_square_to_corpus(struct square_corpus *corpus, struct square *square) {
    // debug("Corpus index %d | Square at %d x %d", corpus->index, square->startX, square->startY);

    if (corpus->index == 30) {
        return false;
    }

    if (corpus->index == 1) {
        struct square *first = corpus->corpus[0];
        if (first->startY != square->startY || first->size != square->size) {
            return false;
        }

        corpus->offset = square->startX - first->startX - first->size;
    }
    
    if (corpus->index >= 2) {
        if (corpus->index % 5 == 0) {
            struct square *prec = corpus->corpus[corpus->index - 5];
            if (
                prec->startX != square->startX ||
                prec->size != square->size ||
                square->startY - prec->startY - prec->size != corpus->offset
            ) {
                return false;
            }
        } else {
            struct square *prec = corpus->corpus[corpus->index - 1];
            if (
                prec->startY != square->startY ||
                prec->size != square->size ||
                square->startX - prec->startX - prec->size != corpus->offset
            ) {
                return false;
            }
        }
    }

    
    corpus->corpus[corpus->index] = square;
    corpus->index++;
    return true;
}

void populate_square_list_from_corpus(struct square_list *sl, struct square_corpus **corpus) {
    for (int i = 0; corpus[i]; i++) {
        for (int j = 0; j < corpus[i]->index; j++) {
            add_square(sl, corpus[i]->corpus[j]);
        }
    }
}

struct square_corpus **find_square_corpus(struct square_list *sl, int maxCorpus) {
    struct square_corpus **corpus_list = calloc(maxCorpus + 1, sizeof(struct square_corpus *));
    debug("Max corpus: %d", maxCorpus);
    int n = 0;

    if (!corpus_list) {
        fatal("OOM while creating Corpus list");
        exit(1);
    }

    while (sl->size) {
        struct square_corpus *corpus = make_square_corpus(sl->head->data);

        for (struct square_list_element *sle = sl->head->next; sle; sle = sle->next) {
            add_square_to_corpus(corpus, sle->data);
        }

        for (int i = 0; i < corpus->index; i++) {
            remove_square(sl, corpus->corpus[i]->startX, corpus->corpus[i]->startY);
        }

        if (corpus->index == 30) {
            corpus_list[n++] = corpus;
        } else {
            free_square_corpus(corpus);
        }

        debug("Corpus number %d", n);
    }

    info("%d corpus found", n);

    return corpus_list;
}