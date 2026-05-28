#include "wordle-grid.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
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
    if (!grids) {
        return;
    }

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
        
        bool lastLine = false;
        for (int y = 0; y < 6; y++) {
            int g = 0;

            if (lastLine) {
                free(grids[i][y]);
                grids[i][y] = NULL;
                continue;
            }

            for (int x = 0; x < 5; x++) {
                struct square *s = sc[i]->corpus[y * 5 + x];
                if (s->color == COLOR_BLACK || s->color == COLOR_WHITE) {
                    lastLine = true;
                    free(grids[i][y]);
                    grids[i][y] = NULL;
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

void print_grids(char ***grids) {
    if (!grids) {
        return;
    }

    for (int i = 0; grids[i]; i++) {
        printf("---- Grid %d ----\n\n", i);
        for (int y = 0; grids[i][y]; y++) {
            printf("%d | %c - %c - %c - %c - %c\n", y, grids[i][y][0], grids[i][y][1], grids[i][y][2], grids[i][y][3], grids[i][y][4]);
        }
        printf("\n");
    }
}


void print_grid(char **grid) {
    if (!grid) {
        return;
    }

    printf("---- Grid ----\n\n");
    for (int y = 0; grid[y]; y++) {
        printf("%d | %c - %c - %c - %c - %c\n", y, grid[y][0], grid[y][1], grid[y][2], grid[y][3], grid[y][4]);
    }
    printf("\n-------------\n");
}