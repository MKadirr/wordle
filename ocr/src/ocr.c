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

void execute(struct options *opts) {
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
        return;
    }

    for (int i = 0; i < nbImages; i++) {
        char *filename = calloc(dirLen + formatLen + numPlaces(i) + 2, sizeof(char));
        sprintf(filename, filenameFormat, i);

        struct png *image = load_png(filename);

        if (!image) {
            free(filename);
            continue;
        }

        image_to_colored_image(image);

        if (opts->withItermediate) {
            char *coloredFilename = calloc(dirLen + numPlaces(i) + 14, sizeof(char));
            sprintf(coloredFilename, "%s/colored-%d.png", opts->imgDir, i);

            save_png(image, coloredFilename);
            free(coloredFilename);
        }

        struct square_list *sl = find_squares(image);
        
        if (opts->withItermediate) {
            image_from_squares(image, sl);

            char *squaredFilename = calloc(dirLen + numPlaces(i) + 18, sizeof(char));
            sprintf(squaredFilename, "%s/raw-squares-%d.png", opts->imgDir, i);

            save_png(image, squaredFilename);
            free(squaredFilename);
        }

        filter_squares(sl, 4, 25);

        if (opts->withItermediate) {
            image_from_squares(image, sl);

            char *filteredSquaredFilename = calloc(dirLen + numPlaces(i) + 23, sizeof(char));
            sprintf(filteredSquaredFilename, "%s/filtered-squares-%d.png", opts->imgDir, i);

            save_png(image, filteredSquaredFilename);
            free(filteredSquaredFilename);
        }

        // Do work

        free_square_list(sl);
        free_png(image);
        free(filename);
    }

    free(filenameFormat);
}

int ocr(int argc, char** argv) {
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

    if (opts->execute) {
        execute(opts);
    }

    free_options(opts);
    free_logger();
}