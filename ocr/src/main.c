#include "ocr.h"
#include "image_processing/image-png.h"
#include "wordle-grid.h"
#include <stdio.h>

int main(int argc, char** argv) {
    char ***grids = ocr(argc, argv);

    print_grids(grids);

    free_wordle_grids(grids);

    return 0;
}