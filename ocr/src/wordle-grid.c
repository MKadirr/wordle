#include "wordle-grid.h"
#include <stdlib.h>
#include <stdbool.h>
#include "logger/logger.h"

char **make_grid() {
    char **grid = calloc(7, sizeof(char *));

    if (!grid) {
        fatal("OOM while creating wordle grid");
        exit(1);
    }

    for (int i = 0; i < 6; i++) {
        grid[i] = calloc(5, sizeof(char));

        if (!grid[i]) {
            for (int j = i - 1; j >= 0; j--) {
                free(grid[j]);
            }

            free(grid);
            fatal("OOM while creating wordle grid line");
            exit(1);
        }
    }

    return grid;
}

void free_wordle_grids(char ***grids) {
    for (int i = 0; grids[i]; i++) {
        for (int j = 0; grids[i][j]; j++) {
            free(grids[i][j]);
        }
        free(grids[i]);
    }
    free(grids);
}

char ***grids_from_corpus(struct square_corpus **sc, int nbCorpus) {
    char ***grids = calloc(nbCorpus + 1, sizeof(char **));

    for (int i; sc[i]; i++) {
        grids[i] = make_grid();
        
        for (int y = 0; y < 6; y++) {
            int g = 0;
            bool lastLine = false;
            for (int x = 0; x < 5; x++) {
                struct square *s = sc[i]->corpus[y * 5 + x];
                if (s->color == COLOR_BLACK || s->color == COLOR_BLACK) {
                    lastLine = true;
                    break;
                }
                
                if (s->color == COLOR_GRAY) {
                    grids[i][y][x] = 'n';
                } else if (s->color == COLOR_YELLOW) {
                    grids[i][y][x] = 'm';
                } else {
                    grids[i][y][x] = 'y';
                }
            }

            if (lastLine) {
                break;
            }
        }
    }

    return grids;
}

char ***merge_grids(char ***g1, char ***g2) {
    if (!g1) {
        return g2;
    }

    if (!g2) {
        return g1;
    }

    int l1 = 0;
    int l2 = 0;

    while(g1[l1]) l1++;
    while(g2[l2]) l2++;

    char ***ng = calloc(l1 + l2 + 1, sizeof(char **));
    for (int i = 0; i < l1; i++) {
        ng[i] = g1[i];
    }

    for (int i = 0; i < l2; i++) {
        ng[i + l1] = g2[i];
    }

    free(g1);
    free(g2);
    return ng;
}