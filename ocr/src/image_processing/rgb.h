#ifndef OCR_RGB_H
#define OCR_RGB_H

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

struct hsv *rgv_to_hsv(struct rgb *pixel);
struct rgb *make_rgb(int r, int g, int b);

#endif /* !OCR_RGB_H */