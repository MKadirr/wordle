#include "image-png.h"
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#include "logger/logger.h"

struct png *load_png(const char *filename) {

    struct png *result = malloc(sizeof(struct png));

    if (!result) {
        fatal("OOM while creating png image for %s", filename);
        exit(1);
    }

    FILE *fp = fopen(filename, "rb");

    if (!fp)
    {
        error("Failed to open file %s", filename);
        return NULL;
    }

    png_structp png =
        png_create_read_struct(
            PNG_LIBPNG_VER_STRING,
            NULL,
            NULL,
            NULL
        );

    if (!png) {
        error("Error while creating png (%s)", filename);
        return NULL;
    }

    png_infop png_info =
        png_create_info_struct(png);

    if (!png_info) {
        error("Error while creating png (%s)", filename);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        error("Error during PNG read (%s)", filename);
        return NULL;
    }

    png_init_io(png, fp);

    png_read_info(png, png_info);

    int width =
        png_get_image_width(png, png_info);

    int height =
        png_get_image_height(png, png_info);

    int color_type =
        png_get_color_type(png, png_info);

    int bit_depth =
        png_get_bit_depth(png, png_info);

    // Convert everything to 8-bit RGBA

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    if (color_type == PNG_COLOR_TYPE_GRAY &&
        bit_depth < 8)
    {
        png_set_expand_gray_1_2_4_to_8(png);
    }

    if (png_get_valid(
            png,
            png_info,
            PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png);
    }

    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_filler(
            png,
            0xFF,
            PNG_FILLER_AFTER
        );
    }

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, png_info);

    png_bytep *rows =
        malloc(sizeof(png_bytep) * height);

    int rowbytes =
        png_get_rowbytes(png, png_info);

    for (int y = 0; y < height; y++)
    {
        rows[y] = malloc(rowbytes);
    }

    png_read_image(png, rows);

    info("Loaded PNG: %dx%d", width, height);

    result->height = height;
    result->width = width;
    result->matrix = calloc(height, sizeof(struct rgba *));
    for (int y = 0; y < height; y++) {
        result->matrix[y] = calloc(width, sizeof(struct rgba));

        for (int x = 0; x < width; x++) {
            png_bytep px = &(rows[y][x]);
            struct rgba *pixel = calloc(1, sizeof(struct rgba));
            pixel->r = px[0];
            pixel->g = px[1];
            pixel->b = px[2];
            pixel->a = px[3];
            result->matrix[y][x] = pixel;
        }
    }

    // Cleanup
    for (int y = 0; y < height; y++)
    {
        free(rows[y]);
    }

    free(rows);

    png_destroy_read_struct(
        &png,
        &png_info,
        NULL
    );

    fclose(fp);

    return result;
}

void free_png(struct png *image) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            free(image->matrix[y][x]);
        }

        free(image->matrix[y]);
    }

    free(image->matrix);
    free(image);
}