#include "word_table.h"

#include <stdlib.h>
#include <string.h>

#define INIT_SZ 256

void word_table_init(word_table_t *wt) {
    wt->entries = malloc(INIT_SZ * sizeof(word_entry_t));
    wt->size = 0;
    wt->cap = INIT_SZ;
}

void word_table_reserve(word_table_t *wt, int cap) {
    if (cap > wt->cap) {
        wt->entries = realloc(wt->entries, cap * 2 * sizeof(word_entry_t));
        wt->cap = cap * 2;
    }
}

void word_table_append(word_table_t *wt, char *word, data_t d) {
    word_table_reserve(wt, wt->size + 1);
    wt->entries[wt->size].word = word;
    wt->entries[wt->size].entry = d;
    wt->size++;
}

void word_table_remove(word_table_t *wt, char *word) {
    for (int i = wt->size - 1; i >= 0; i--) {
        if (strcmp(wt->entries[i].word, word) == 0) {
            data_t entry = wt->entries[i].entry;
            if (entry >= 0) {
                wt->entries[i].entry = -1;
                return;
            }
        }
    }
}

data_t word_table_find(word_table_t *wt, char *word) {
    for (int i = wt->size - 1; i >= 0; i--) {
        if (strcmp(wt->entries[i].word, word) == 0) {
            data_t entry = wt->entries[i].entry;
            if (entry >= 0)
                return entry;
        }
    }
    return -1;
}
