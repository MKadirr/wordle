#include "process-image.h"

void image_to_colored_image(struct png *image) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            rgb_to_color_rgb(image->matrix[y][x]);
        }
    }
}