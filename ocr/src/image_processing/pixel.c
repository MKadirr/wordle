#include "pixel.h"
#include <stdlib.h>
#include "logger/logger.h"

float fmodf(float x, float y) {
    int quotient = (int)(x / y);
    return x - quotient * y;
}

struct rgb *make_rgb(int r, int g, int b) {
    struct rgb *pixel = calloc(1, sizeof(struct rgb));

    if (!pixel) {
        fatal("OOM while creating RGB");
        exit(1);
    }

    pixel->r = r;
    pixel->g = g;
    pixel->b = b;
    return pixel;
}

struct hsv *make_hsv(float h, float s, float v) {
    struct hsv *pixel = calloc(1, sizeof(struct hsv));

    if (!pixel) {
        fatal("OOM while creating HSV");
        exit(1);
    }

    pixel->h = h;
    pixel->s = s;
    pixel->v = v;
    return pixel;
}

struct hsv *rgb_to_hsv(struct rgb *pixel) {
    struct hsv *out = make_hsv(0, 0, 0);

    float r = pixel->r / 255.0f;
    float g = pixel->g / 255.0f;
    float b = pixel->b / 255.0f;

    float min = r < g ? (r < b ? r : b) : (g < b ? g : b);
    float max = r > g ? (r > b ? r : b) : (g > b ? g : b);

    float delta = max - min;

    out->v = max;
    
    if (max == 0.0f) {
        out->s = 0.0f;
    } else {
        out->s = delta / max;
    }

    if (delta == 0.0f) {
        out->h = 0.0f;
    } else if (max == r) {
        out->h = 60.0f * fmodf((g - b) / delta, 6.0f);
    } else if (max == g) {
        out->h = 60.0f * (((b - r) / delta) + 2.0f);
    } else {
        out->h = 60.0f * (((r - g) / delta) + 4.0f);
    }

    if (out->h < 0.0f) {
        out->h += 360.0f;
    }

    return out;
}

void change_pixel_color(struct rgb *pixel, enum color col) {
    switch (col) {
        case COLOR_BLACK:
            pixel->r = 0;
            pixel->g = 0;
            pixel->b = 0;
            break;
        
        case COLOR_WHITE:
            pixel->r = 255;
            pixel->g = 255;
            pixel->b = 255;
            break;
    
        case COLOR_GRAY:
            pixel->r = 120;
            pixel->g = 120;
            pixel->b = 120;
            break;

        case COLOR_YELLOW:
            pixel->r = 180;
            pixel->g = 160;
            pixel->b = 60;
            break;

        case COLOR_GREEN:
            pixel->r = 80;
            pixel->g = 140;
            pixel->b = 80;
            break;
    }
}

enum color rgb_color(struct rgb *pixel) {
    if (pixel->r == 255 && pixel->g == 255 && pixel->b == 255) {
        return COLOR_WHITE;
    }

    if (pixel->r == 120 && pixel->g == 120 && pixel->b == 120) {
        return COLOR_GRAY;
    }

    if (pixel->r == 180 && pixel->g == 160 && pixel->b == 60) {
        return COLOR_YELLOW;
    }

    if (pixel->r == 80 && pixel->g == 140 && pixel->b == 80) {
        return COLOR_GREEN;
    }

    return COLOR_BLACK;
}

void rgb_to_color_rgb(struct rgb *pixel) {
    struct hsv *phsv = rgb_to_hsv(pixel);

    if (phsv->s < 0.15f) {
        if (phsv->v < 0.2f) {
            change_pixel_color(pixel, COLOR_BLACK);
        } else change_pixel_color(pixel, COLOR_GRAY);
        free(phsv);
        return;
    }

    if (30 <= phsv->h < 90) {
        change_pixel_color(pixel, COLOR_YELLOW);
    } else if (90 <= phsv->h < 150) {
        change_pixel_color(pixel, COLOR_GREEN);
    } else {
        change_pixel_color(pixel, COLOR_BLACK);
    }
    
    free(phsv);
}