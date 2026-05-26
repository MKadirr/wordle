#ifndef OCR_PIXEL_H
#define OCR_PIXEL_H

struct rgb {
    int r;
    int g;
    int b;
};

struct hsv {
    float h;
    float s;
    float v;
};

enum color {
    COLOR_BLACK = -2,
    COLOR_WHITE = -1,
    COLOR_GRAY = 0,
    COLOR_YELLOW = 1,
    COLOR_GREEN = 2
};

struct hsv *rgb_to_hsv(struct rgb *pixel);
struct rgb *make_rgb(int r, int g, int b);
void change_pixel_color(struct rgb *pixel, enum color col);
enum color rgb_color(struct rgb *pixel);
enum color rgb_to_color_rgb(struct rgb *pixel);

#endif /* !OCR_PIXEL_H */