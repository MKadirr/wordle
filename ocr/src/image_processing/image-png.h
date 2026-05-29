#ifndef OCR_IMAGE_PNG_H
#define OCR_IMAGE_PNG_H

#include "pixel.h"

struct png {
    struct rgb ***matrix;
    int width;
    int height;
};

struct png *load_png(const char *filename);
void save_png(struct png *image, const char *filename);
void free_png(struct png *image);

#endif /* !OCR_IMAGE_PNG_H */