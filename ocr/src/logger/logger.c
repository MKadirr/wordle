#include "logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

struct logger* LOGGER = NULL;

void init_logger(enum LOG_LEVEL init_lvl) {
    LOGGER = calloc(1, sizeof(struct logger));
    if (!LOGGER) {
        fprintf(stderr, "OOM: Can't create logger");
        exit(1);
    }
    LOGGER->lvl = init_lvl;
}

void free_logger() {
    free(LOGGER);
}

void debug(const char *fmt, ...) {
    if (!LOGGER || LOGGER->lvl != LOG_DEBUG) {
        return;
    }

    va_list args;
    va_start(args, fmt);

    printf("\033[32m[DEBUG] ");
    vprintf(fmt, args);
    printf("\033[0m\n");

    va_end(args);
}

void info(const char *fmt, ...) {
    if (!LOGGER || LOGGER->lvl < LOG_INFO) {
        return;
    }

    va_list args;
    va_start(args, fmt);

    printf("\033[34m[INFO] ");
    vprintf(fmt, args);
    printf("\033[0m\n");

    va_end(args);
}

void warn(const char *fmt, ...) {
    if (!LOGGER || LOGGER->lvl < LOG_WARN) {
        return;
    }

    va_list args;
    va_start(args, fmt);

    printf("\033[33m[WARN] ");
    vprintf(fmt, args);
    printf("\033[0m\n");

    va_end(args);
}

void error(const char *fmt, ...) {
    if (!LOGGER || LOGGER->lvl < LOG_ERROR) {
        return;
    }

    va_list args;
    va_start(args, fmt);

    printf("\033[1;31m[ERROR] ");
    vprintf(fmt, args);
    printf("\033[0m\n");

    va_end(args);
}

void fatal(const char *fmt, ...) {
    if (!LOGGER) {
        return;
    }

    va_list args;
    va_start(args, fmt);

    printf("\033[31m[FATAL] ");
    vprintf(fmt, args);
    printf("\033[0m\n");

    va_end(args);
}

char *log_level_to_string(enum LOG_LEVEL lvl) {
    switch (lvl) {
        case LOG_DEBUG:
            return "debug";
        case LOG_INFO:
            return "info";
        case LOG_WARN:
            return "warn";
        case LOG_ERROR:
            return "error";
        case LOG_FATAL:
            return "fatal";
    }
}

void set_level(enum LOG_LEVEL new_lvl) {
    if (LOGGER) {
        LOGGER->lvl = new_lvl;
    }
}