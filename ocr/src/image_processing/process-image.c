#include "process-image.h"
#include "logger/logger.h"
#include <stdlib.h>

void image_to_colored_image(struct png *image) {
    int colors[5] = { 0 };
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            enum color color = rgb_to_color_rgb(image->matrix[y][x]);
            colors[color + 2]++;
        }
    }

    debug("BLACK: %d | WHITE: %d | GRAY: %d | YELLOW: %d | GREEN: %d", colors[0], colors[1], colors[2], colors[3], colors[4]);
}

void fill_image(struct png *image, int startX, int startY, int size, enum color color) {
    for (int x = startX; x < startX + size; x++) {
        for (int y = startY; y < startY + size; y++) {
            change_pixel_color(image->matrix[y][x], color);
        }
    }
}

struct square *find_square(int x, int y, struct png *image) {
    return NULL;
}

struct square_list *find_squares(struct png *image) {
    struct square_list *sl = make_square_list();

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            struct square *s = find_square(x, y, image);
            if (s) {
                add_square(sl, s);
                fill_image(image, s->startX, s->startY, s->size, COLOR_BLACK);
            }
        }
    }
}