#include "process-image.h"
#include "logger/logger.h"
#include <stdlib.h>
#include "counter/counter.h"

void image_to_colored_image(struct png *image) {
    int colors[5] = { 0 };
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            enum color color = rgb_to_color_rgb(image->matrix[y][x]);
            colors[color + 2]++;
        }
    }

    debug("Pixel colors: BLACK %d | WHITE %d | GRAY %d | YELLOW %d | GREEN %d", colors[0], colors[1], colors[2], colors[3], colors[4]);
}

void fill_image(struct png *image, int startX, int startY, int size, enum color color) {
    for (int x = startX; x < startX + size; x++) {
        for (int y = startY; y < startY + size; y++) {
            change_pixel_color(image->matrix[y][x], color);
        }
    }
}

struct square *find_gray_square(int x, int y, struct png *image) {
    int sx = 0;
    int sy = 0;
    while (image->width > x + sx && rgb_color(image->matrix[y][x + sx]) == COLOR_GRAY) {
        sx += 1;
    }

    while (image->height > y + sy && rgb_color(image->matrix[y + sy][x + sx - 1]) == COLOR_GRAY) {
        sy += 1;
    }

    if (sx != sy) {
        return NULL;
    }

    for (int xi = x; xi < x + sx; xi++) {
        for (int yi = y; yi < y + sy; yi++) {
            enum color icolor = rgb_color(image->matrix[yi][xi]);
            if (icolor != COLOR_GRAY && icolor != COLOR_BLACK) {
                return NULL;
            }
        }
    }
    
    int midX = x + (int)(sx / 2);
    int midY = y + (int)(sy / 2);

    enum color mcolor = rgb_color(image->matrix[midY][midX]);
    if (mcolor == COLOR_BLACK) {
        mcolor = COLOR_WHITE;
    }

    return make_square(x, y, sx, mcolor);
}

struct square *find_mono_color_square(int x, int y, struct png *image, enum color color) {
    int sx = 1;
    int sy = 1;
    while (image->width > x + sx && image->height > y + sy && rgb_color(image->matrix[y + sy][x + sx]) == color) {
        sx += 1;
        sy += 1;
    }
    
    for (int xi = x; xi < x + sx; xi++) {
        for (int yi = y; yi < y + sy; yi++) {
            enum color icolor = rgb_color(image->matrix[yi][xi]);
            if (icolor != color) {
                return NULL;
            }
        }
    }
    
    return make_square(x, y, sx, color);
}

struct square *find_square(int x, int y, struct png *image, enum color color) {
    // debug("Find square from %d %d with color %d", x, y, color);
    if (color == COLOR_GRAY) {
        return find_gray_square(x, y, image);
    }
    return find_mono_color_square(x, y, image, color);
}

struct square_list *find_squares(struct png *image) {
    debug("Start find squares");
    struct square_list *sl = make_square_list();

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            enum color color = rgb_color(image->matrix[y][x]);
            if (color != COLOR_BLACK) {
                struct square *s = find_square(x, y, image, color);
                if (s) {
                    add_square(sl, s);
                    fill_image(image, s->startX, s->startY, s->size, COLOR_BLACK);
                }
            }
        }
    }

    info("Squares found: %d", square_list_len(sl));
    return sl;
}

void image_from_squares(struct png *image, struct square_list *sl) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            change_pixel_color(image->matrix[y][x], COLOR_BLACK);
        }
    }

    struct square_list_element *sle = sl->head;
    while (sle) {
        fill_image(image, sle->data->startX, sle->data->startY, sle->data->size, sle->data->color);
        sle = sle->next;
    }
}

void filter_squares(struct square_list *sl, int minSize, int maxSize) {
    struct square_list_element *sle = sl->head;
    struct counter *c = make_counter(minSize, maxSize);
    if (!c) {
        return;
    }

    while (sle) {
        struct square_list_element *next = sle->next;
        if (sle->data->size < minSize || sle->data->size > maxSize) {
            // debug("Remove Square [%d, %d] of size: %d", sle->data->startX, sle->data->startY, sle->data->size);
            free(remove_square(sl, sle->data->startX, sle->data->startY));
        } else {
            add_to_counter(c, sle->data->size);
        }
        sle = next;
    }

    sle = sl->head;
    while (sle) {
        struct square_list_element *next = sle->next;
        if (count_of(c, sle->data->size) < 30) {
            free(remove_square(sl, sle->data->startX, sle->data->startY));
        }
        sle = next;
    }

    free_counter(c);
}