#ifndef OCR_CORPUS_H
#define OCR_CORPUS_H

#include "square.h"

struct square_corpus {
    struct square *corpus[6][5];
    int index;
};

struct square_corpus *make_square_corpus(struct square *first);
void free_square_corpus(struct square_corpus *sc);

#endif /* !OCR_CORPUS_H */