#include "wordle.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void auto_fill(char* buffer, char* expect, char* result) {
    assert(buffer != NULL);
    assert(expect != NULL);
    assert(result != NULL);

    char tmp[WORD_SIZE + 1];
    for (size_t i = 0; i < WORD_SIZE; i++) {
        tmp[i] = expect[i];
    }
    
    for (int i = 0; i < WORD_SIZE; i++) {
        if (buffer[i] == expect[i]) {
            result[i] = 'y';
        }
        else {
            int find = 0;
            for (int j = 0; j < WORD_SIZE; j++) {
                if (i != j && expect[j] == buffer[i] && buffer[j] != expect[j]) {
                    find += 1;
                    expect[j] = 0;
                    break;
                }
            }

            result[i] = find ? 'm' : 'n';
        }
    }

    for (size_t i = 0; i < WORD_SIZE; i++) {
        expect[i] = tmp[i];
    }
}

int compact(char a) {
    return 1 << (a % 32);
}