#ifndef OCR_IMAGE_PNG
#define OCR_IMAGE_PNG

#include "rgb.h"

struct png {
    struct rgb ***matrix;
    int width;
    int height;
};

struct png *load_png(const char *filename);
void free_png(struct png *image);

#endif /* !OCR_IMAGE_PNG */