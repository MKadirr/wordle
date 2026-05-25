#include "utils.h"
#include <sys/stat.h>

int numPlaces (int n) {
    int r = 1;
    while (n > 9) {
        n /= 10;
        r++;
    }
    return r;
}

bool file_exists(const char *filename) {
    struct stat buffer;
    return stat(filename, &buffer) == 0 ? true : false;
}