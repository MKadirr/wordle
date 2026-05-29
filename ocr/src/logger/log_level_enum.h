#ifndef OCR_LOG_LEVEL_ENUM_H
#define OCR_LOG_LEVEL_ENUM_H

enum LOG_LEVEL {
    LOG_DEBUG = 4,
    LOG_INFO = 3,
    LOG_WARN = 2,
    LOG_ERROR = 1,
    LOG_FATAL = 0
};

char *log_level_to_string(enum LOG_LEVEL lvl);

#endif /* !LOG_LEVEL_ENUM_H */