#ifndef OCR_IMAGE_PNG
#define OCR_IMAGE_PNG

struct rgba {
    int r;
    int g;
    int b;
    int a;
};

struct png {
    struct rgba ***matrix;
    int width;
    int height;
};

struct png *load_png(const char *filename);
void free_png(struct png *image);

#endif /* !OCR_IMAGE_PNG */