#ifndef OCR_LOGGER_H
#define OCR_LOGGER_H

#include "log_level_enum.h"

struct logger {
    enum LOG_LEVEL lvl;
};

void debug(const char *fmt, ...);
void info(const char *fmt, ...);
void warn(const char *fmt, ...);
void error(const char *fmt, ...);
void fatal(const char *fmt, ...);

void init_logger(enum LOG_LEVEL init_lvl);
void free_logger();
void set_level(enum LOG_LEVEL new_lvl);

#endif /* !LOGGER_H */