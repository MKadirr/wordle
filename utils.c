#include "wordle.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define DATASET_FILE "valid-wordle-words.txt"
#define USED_FILE "word-bank.csv"

void auto_fill(char* buffer, char* expect, char* result) {
    assert(buffer != NULL);
    assert(expect != NULL);
    assert(result != NULL);
    
    for (int i = 0; i < WORD_SIZE; i++) {
        if (buffer[i] == expect[i]) {
            result[i] = 'y';
        }
        else {
            int find = 0;
            for (int j = 0; j < WORD_SIZE; j++) {
                if (i != j && expect[j] == buffer[i] && buffer[j] != expect[j]) {
                    find = 1;
                }
            }

            result[i] = find ? 'm' : 'n';
        }
    }
}