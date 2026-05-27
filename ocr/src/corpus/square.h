#ifndef OCR_SQUARE_H
#define OCR_SQUARE_H

#include "image_processing/pixel.h"

struct square {
    int startX;
    int startY;
    int size;
    enum color color;
};

struct square_list_element {
    struct square *data;
    struct square_list_element *next;
};

struct square_list {
    struct square_list_element *head;
};

struct square *make_square(int startX, int startY, int size, enum color color);
struct square_list *make_square_list(void);
void add_square(struct square_list *s, struct square *square);
struct square *remove_square(struct square_list *s, int startX, int startY);
int square_list_len(struct square_list *sl);
void free_square_list(struct square_list *s);
void free_square(struct square *s);

#endif /* !OCR_SQUARE_H */