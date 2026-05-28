#include "options.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "logger/logger.h"

char *HELP = "\n\
Usage: %s [options]\n\
\n\
Options:\n\
    --help                                    Print this tutorial\n\
    -l {LOG_LEVEL} | --log-level {LOG_LEVEL}  Set the logging level ('degub' / 'info' / 'warn' / 'error' / 'fatal')\n\
    -d {DIR} | --img-dir {DIR}                Set the images directory to {DIR} (Default is 'images')\n\
    -n {NAME} | --img-name-format {NAME}      Give a name format for the image. It must be like this: '.*%%d.*\\.png', %%d will be replace by the index of the image.\n\
    -o {NAME} | --output-format {NAME}        Like --img-name-format for output images.\n\
    -t {TYPE} | --data-type {TYPE}            Choose the result datatype between MATRIX or LIST (MATRIX by default)\n\
    -b {PATH} | --bash {PATH}                 Set the path to the bash executable\n\
    -i | --with-intermediate                  Also save itermediate preproccessed images\n\n\
    --local                                   If you want to execute the program in the ocr folder, otherwise must be executed in the parent folder.\n\
    --no-curl                                 Don't curl the images, instead use the already present images.\n\
";

bool is_valid_file_format(const char *fileFormat) {
    int len = strlen(fileFormat);

    if (
        len < 4 ||
        fileFormat[len - 1] != 'g' ||
        fileFormat[len - 2] != 'n' ||
        fileFormat[len - 3] != 'p' ||
        fileFormat[len - 4] != '.'
    ) {
        error("File format must end with .png");
        return false;
    }

    for (int i = 0; i < len - 4; i++) {
        if (fileFormat[i] == '%' && fileFormat[i + 1] == 'd') {
            return true;
        }
    }

    error("File format must contain %%d");
    return false;
}

struct options *make_options(void) {
    struct options *opts = malloc(sizeof(struct options));

    if (!opts) {
        fatal("OOM while creating options");
        exit(0);
    }

    opts->misformed = false;
    opts->execute = true;
    opts->logLvl = LOG_WARN;
    opts->imgDir = "images";
    opts->nameFormat = "Image-%d.png";
    opts->outputFormat = "output-%d.png";
    opts->dataType = DATA_TYPE_MATRIX;
    opts->bashExec = "bash";
    opts->withItermediate = false;
    opts->localExec = false;
    opts->noCurl = false;

    return opts;
}

void free_options(struct options *opts) {
    free(opts);
}

void print_help(char *program) {
    printf(HELP, program);
}

struct options *parse_options(int argc, char** argv) {
    struct options *opts = make_options();

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            print_help(argv[0]);
            opts->execute = false;
            return opts;
        } 
        
        else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--log-level")) {
            if (++i >= argc) {
                error("No log level after %s", argv[i - 1]);
                opts->misformed = true;
                return opts;
            }

            if (!strcmp(argv[i], "debug")) {
                opts->logLvl = LOG_DEBUG;
            } else if (!strcmp(argv[i], "info")) {
                opts->logLvl = LOG_INFO;
            } else if (!strcmp(argv[i], "warn")) {
                opts->logLvl = LOG_WARN;
            } else if (!strcmp(argv[i], "error")) {
                opts->logLvl = LOG_ERROR;
            } else if (!strcmp(argv[i], "fatal")) {
                opts->logLvl = LOG_FATAL;
            } else {
                error("Invalid log level %s", argv[i]);
                opts->misformed = true;
                return opts;
            }
        } 
        
        else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--img-dir")) {
            if (++i >= argc) {
                error("No directory specify after %s", argv[i - 1]);
                opts->misformed = true;
                return opts;
            }

            opts->imgDir = argv[i];
        } 
        
        else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--img-name-format")) {
            if (++i >= argc) {
                error("No name format specify after %s", argv[i - 1]);
                opts->misformed = true;
                return opts;
            }

            if (!is_valid_file_format(argv[i])) {
                opts->misformed = true;
                return opts;
            }

            opts->nameFormat = argv[i];
        }

        else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output-format")) {
            if (++i >= argc) {
                error("No output format name specify after %s", argv[i - 1]);
                opts->misformed = true;
                return opts;
            }

            if (!is_valid_file_format(argv[i])) {
                opts->misformed = true;
                return opts;
            }

            opts->outputFormat = argv[i];
        }

        else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--data-type")) {
            if (++i >= argc) {
                error("No data type specify after %s", argv[i - 1]);
                opts->misformed = true;
                return opts;
            }

            if (!strcmp(argv[i], "MATRIX") || !strcmp(argv[i], "matrix") || !strcmp(argv[i], "Matrix")) {
                opts->dataType = DATA_TYPE_MATRIX;
            } else if (!strcmp(argv[i], "LIST") || !strcmp(argv[i], "list") || !strcmp(argv[i], "List")) {
                opts->dataType = DATA_TYPE_LIST;
            } else {
                error("Invalid data type  %s", argv[i]);
                opts->misformed = true;
                return opts;
            }
        }

        else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bash")) {
            if (++i >= argc) {
                error("No bash path specify after %s", argv[i - 1]);
                opts->misformed = true;
                return opts;
            }

            opts->bashExec = argv[i];
        }

        else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--with-intermediate")) {
            opts->withItermediate = true;
        }

        else if (!strcmp(argv[i], "--local")) {
            opts->localExec = true;
        }

        else if (!strcmp(argv[i], "--no-curl")) {
            opts->noCurl = true;
        }

        else {
            opts->misformed = true;
            error("Invalid options %s", argv[i]);
            return opts;
        }
    }

    return opts;
}