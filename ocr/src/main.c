#include "ocr.h"
#include "image_processing/image-png.h"

int main(int argc, char** argv) {
    struct png *image = load_png("Image-0.png");

    free_png(image);

    return ocr(argc, argv);
}