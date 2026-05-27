#ifndef OCR_PROCESS_IMAGE_H
#define OCR_PROCESS_IMAGE_H

#include "image-png.h"
#include "corpus/square.h"

void image_to_colored_image(struct png *image);
struct square_list *find_squares(struct png *image);
void fill_image(struct png *image, int startX, int startY, int size, enum color color);
void image_from_squares(struct png *image, struct square_list *sl);
void filter_squares(struct square_list *sl, int minSize, int maxSize);

#endif /* !OCR_PROCESS_IMAGE_H */