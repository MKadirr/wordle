#ifndef OCR_WORDLE_GRID_H
#define OCR_WORDLE_GRID_H

#include "corpus/corpus.h"

/**
 * Convert corpus to a pratical wordle grid.
 * Return a struct wordle grid NULL terminate array;
 */
char ***grids_from_corpus(struct square_corpus **sc, int nbCorpus);
char ***merge_grids(char ***g1, char ***g2);
void print_grids(char ***grids);
void print_grid(char **grid);
void free_wordle_grids(char ***grids);

#endif /* !OCR_WORDLE_GRID_H */