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


int get_current_images(struct options *opts) {
    info("Skip curl, simply count already present images with %s pattern.", opts->nameFormat);

    int formatLen = strlen(opts->nameFormat) - 2;
    int dirLen = strlen(opts->imgDir);

    char *filenameFormat = calloc(dirLen + formatLen + 4, sizeof(char));
    sprintf(filenameFormat, "%s/%s", opts->imgDir, opts->nameFormat);
    debug("Filename format: %s", filenameFormat);

    int nbImages = -1;
    bool exists = true;

    while (exists) {
        nbImages++;

        char *filename = calloc(dirLen + formatLen + numPlaces(nbImages) + 2, sizeof(char));
        sprintf(filename, filenameFormat, nbImages);
        debug("Search for %s", filename);

        exists = file_exists(filename);

        free(filename);
    }

    free(filenameFormat);
    info("%d images found.", nbImages);
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
    } else {
        nbImages = get_current_images(opts);
    }

    if (nbImages <= 0) {
        return;
    }
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