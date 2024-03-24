#ifndef REINFORTH_TYPES_H_
#define REINFORTH_TYPES_H_

#include "stdint.h"

struct forthvm;
typedef struct forthvm forthvm_t;

typedef intptr_t data_t;

struct word_entry {
    char *word;
    data_t entry;
};
typedef struct word_entry word_entry_t;

typedef void (*opfunc)(forthvm_t *);

#endif
