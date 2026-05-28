#include "ocr.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#include "options/options.h"
#include "logger/logger.h"
#include "utils/utils.h"
#include "image_processing/image-png.h"
#include "image_processing/process-image.h"
#include "corpus/square.h"
#include "corpus/corpus.h"
#include "wordle-grid.h"

int get_current_images(struct options *opts, const char *filenameFormat, int filenameFormatLen) {
    info("Skip curl, simply count already present images with %s pattern.", opts->nameFormat);

    int nbImages = -1;
    bool exists = true;

    while (exists) {
        nbImages++;

        char *filename = calloc(filenameFormatLen + numPlaces(nbImages) + 2, sizeof(char));
        sprintf(filename, filenameFormat, nbImages);
        debug("Search for %s", filename);

        exists = file_exists(filename);

        free(filename);
    }

    info("%d images found.", nbImages);
    return nbImages;
}

int call_curl(struct options *opts) {
    pid_t pid = fork();

    if (pid == -1) {
        error("Fork failed");
        return -1;
    }

    if (pid == 0) {
        char *argv[] = { opts->bashExec, opts->localExec ? "./curl.sh" : "./ocr/curl.sh", log_level_to_string(opts->logLvl), opts->imgDir, opts->nameFormat, NULL };
        debug("Argv: \n\t- Program name: %s\n\tLog level: %s\n\tImage directory: %s\n\tName format: %s", argv[1], argv[2], argv[3], argv[4]);

        execvp(opts->bashExec, argv);

        error("Execvp failed");
        exit(1);
    }
    else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            info("Curl program end");

            int code = WEXITSTATUS(status);

            debug("Received number of images: %d", code);
            return code;
        }

        error("Curl program end with an issue");
    }

    return -1;
}

char ***_worlde_grid_from_image(char *filename, struct options *opts, int i) {
    bool freeOptions = false;
    if (!opts) {
        freeOptions = true;
        opts = make_options();
    }

    int dirLen = strlen(opts->imgDir);
    int ni = numPlaces(i);

    struct png *image = load_png(filename);

    if (!image) {
        free(filename);
    }

    image_to_colored_image(image);

    if (opts->withItermediate) {
        char *coloredFilename = calloc(dirLen + ni + 14, sizeof(char));
        sprintf(coloredFilename, "%s/colored-%d.png", opts->imgDir, i);

        save_png(image, coloredFilename);
        free(coloredFilename);
    }

    struct square_list *sl = find_squares(image);
        
    if (opts->withItermediate) {
        image_from_squares(image, sl);

        char *squaredFilename = calloc(dirLen + ni + 18, sizeof(char));
        sprintf(squaredFilename, "%s/raw-squares-%d.png", opts->imgDir, i);

        save_png(image, squaredFilename);
        free(squaredFilename);
    }

    int maxCorpus = filter_squares(sl, 4, 25);

    info("The max number of corpus is: %d", maxCorpus);

    if (opts->withItermediate) {
        image_from_squares(image, sl);

        char *filteredSquaredFilename = calloc(dirLen + ni + 23, sizeof(char));
        sprintf(filteredSquaredFilename, "%s/filtered-squares-%d.png", opts->imgDir, i);

        save_png(image, filteredSquaredFilename);
        free(filteredSquaredFilename);
    }

    struct square_corpus **corpus = find_square_corpus(sl, &maxCorpus);
        
    populate_square_list_from_corpus(sl, corpus);
    image_from_squares(image, sl);

    if (opts->saveOutput) {

        int outputFilenameFormatLen = dirLen + strlen(opts->outputFormat) + 1;
        char *outputFilenameFormat = calloc(outputFilenameFormatLen + 1, sizeof(char));
        sprintf(outputFilenameFormat, "%s/%s", opts->imgDir, opts->outputFormat);
        char *outputFilename = calloc(outputFilenameFormatLen + ni - 1, sizeof(char));
        sprintf(outputFilename, outputFilenameFormat, i);

        save_png(image, outputFilename);
        free(outputFilenameFormat);
        free(outputFilename);
    }

    char ***grids = grids_from_corpus(corpus, maxCorpus);

    for (int i = 0; corpus[i]; i++) {
        free(corpus[i]);
    }
    free(corpus);

    free_square_list(sl);
    free_png(image);

    if (freeOptions) {
        free_options(opts);
    }

    return grids;
}

char ***execute(struct options *opts) {
    debug("Start execution");

    int nbImages;
    if (!opts->noCurl) {
        nbImages = call_curl(opts);
    }

    int formatLen = strlen(opts->nameFormat) - 2;
    int dirLen = strlen(opts->imgDir);

    char *filenameFormat = calloc(dirLen + formatLen + 4, sizeof(char));
    sprintf(filenameFormat, "%s/%s", opts->imgDir, opts->nameFormat);
    debug("Filename format: %s", filenameFormat);

    if (opts->noCurl) {
        nbImages = get_current_images(opts, filenameFormat, dirLen + formatLen);
    }

    debug("Image of number: %d", nbImages);

    if (nbImages <= 0) {
        free(filenameFormat);
        return NULL;
    }

    char ***result = NULL;

    for (int i = 0; i < nbImages; i++) {
        int ni = numPlaces(i);
        char *filename = calloc(dirLen + formatLen + ni + 2, sizeof(char));
        sprintf(filename, filenameFormat, i);
        
        char ***cgrids = _worlde_grid_from_image(filename, opts, i);
        result = merge_grids(result, cgrids);
        
        free(filename);
    }

    free(filenameFormat);
    return result;
}

char ***worlde_grid_from_image(char *filename) {
    init_logger(LOG_WARN);
    char *** grids = _worlde_grid_from_image(filename, NULL, 0);
    free_logger();
    return grids;
}

char ***ocr(int argc, char** argv) {
    init_logger(LOG_WARN);
    struct options* opts = parse_options(argc, argv);

    if (opts->misformed) {
        error("--- Misformed options ---");
        print_help(argv[0]);
        free_options(opts);
        free_logger();
        return 0;
    }

    set_level(opts->logLvl);

    char ***grids = NULL;
    if (opts->execute) {
        grids = execute(opts);
    }

    free_options(opts);
    free_logger();
    return grids;
}