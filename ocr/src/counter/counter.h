#ifndef OCR_COUNTER_H
#define OCR_COUNTER_H

struct counter {
    int minSize;
    int maxSize;
    int *numbers;
};

struct counter *make_counter(int minSize, int maxSize);
void add_to_counter(struct counter *c, int size);
int count_of(struct counter *c, int size);
void free_counter(struct counter *c);

#endif /* !OCR_COUNTER_H */