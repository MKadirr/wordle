#ifndef OCR_OPTIONS_H
#define OCR_OPTIONS_H

#include "logger/log_level_enum.h"
#include "data_type.h"
#include <stdbool.h>

struct options {
    bool misformed;
    bool execute;
    enum LOG_LEVEL logLvl;
    char* imgDir;
    char* nameFormat;
    const char* outputFormat;
    char* bashExec;
    enum data_type dataType;
    bool withItermediate;
    bool localExec;
    bool noCurl;
};

struct options *make_options(void);
void free_options(struct options *opts);
struct options *parse_options(int argc, char** argv);
void print_help(char *program);

#endif /* !OCR_OPTIONS_H */