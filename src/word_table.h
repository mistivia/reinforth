#ifndef HTABLE_H_
#define HTABLE_H_

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

typedef struct {
    word_entry_t *entries;
    int size;
    int cap;
} word_table_t;

void word_table_init(word_table_t *wt);
void word_table_append(word_table_t *ht, char *word, data_t d);
void word_table_remove(word_table_t *ht, char *word);
data_t word_table_find(word_table_t *ht, char *word);

#endif
