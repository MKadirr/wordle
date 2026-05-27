#include "wordle-grid.h"
#include <stdlib.h>
#include "logger/logger.h"

struct worlde_grid *make_grid() {
    struct worlde_grid *grid = calloc(1, sizeof(struct worlde_grid));

    if (!grid) {
        fatal("OOM while creating wordle grid");
        exit(1);
    }

    return grid;
}

void free_worlde_grids(struct worlde_grid **grids) {
    for (int i = 0; grids[i]; i++) {
        free(grids[i]);
    }
    free(grids);
}

struct worlde_grid **grids_from_corpus(struct square_corpus **sc, int nbCorpus) {
    struct worlde_grid **grids = calloc(nbCorpus + 1, sizeof(struct worlde_grid *));

    for (int i; sc[i]; i++) {
        grids[i] = make_grid();
        
        for (int y = 0; y < 6; y++) {
            int g = 0;
            for (int x = 0; x < 5; x++) {
                struct square *s = sc[i]->corpus[y * 5 + x];
                if (s->color == COLOR_BLACK || s->color == COLOR_BLACK) {
                    g = -1;
                    break;
                }
                /*
                    Here we have:
                        0 for Gray squares
                        1 for Yellow squares
                        2 for Green squares
                    The line is -1 if there is no data (Black or white) 
                */
                g += s->color << 3 * x;
            }
            grids[y] = g;
        }
    }

    return grids;
}