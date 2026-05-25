#include "rgb.h"
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

struct hsv *rgv_to_hsv(struct rgb *pixel) {
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