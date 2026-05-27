#ifndef OCR_WORDLE_GRID_H
#define OCR_WORDLE_GRID_H

#include "corpus/corpus.h"

#define WG1 0b111
#define WG2 0b111000
#define WG3 0b111000000
#define WG4 0b111000000000
#define WG5 0b111000000000000

struct worlde_grid {
    int grid[6];
};

/**
 * Convert corpus to a pratical wordle grid.
 * Return a struct wordle grid NULL terminate array;
 */
struct worlde_grid **grids_from_corpus(struct square_corpus **sc);
void free_worlde_grids(struct worlde_grid **grids);

#endif /* !OCR_WORDLE_GRID_H */