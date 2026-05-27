#ifndef OCR_CORPUS_H
#define OCR_CORPUS_H

#include "square.h"

struct square_corpus {
    struct square *corpus[30];
    int index;
    int offset;
};

struct square_corpus *make_square_corpus(struct square *first);
struct square_corpus **find_square_corpus(struct square_list *sl, int maxCorpus);
void populate_square_list_from_corpus(struct square_list *sl, struct square_corpus **curpus);
void free_square_corpus(struct square_corpus *sc);

#endif /* !OCR_CORPUS_H */