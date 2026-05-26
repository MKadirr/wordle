#ifndef OCR_SQUARE_H
#define OCR_SQUARE_H

#include "image_processing/pixel.h"

struct square {
    int startX;
    int startY;
    int size;
    enum color color;
};

struct square *make_square(int startX, int startY, int size, enum color color);
void free_square(struct square *s);

#endif /* !OCR_SQUARE_H */