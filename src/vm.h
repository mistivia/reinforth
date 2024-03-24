#ifndef REINFORTH_VM_H_
#define REINFORTH_VM_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "opcode.h"
#include "syntax.h"
#include "types.h"
#include "word_table.h"

struct forthvm {
    data_t *ds;
    data_t *rs;
    void *heap;
    data_t *dict;
    data_t *code;
    word_table_t *wordtable;

    data_t pc;
    data_t dsp;
    data_t rsp;
    void *heaptop;
    data_t ret;

    data_t codesz;
    data_t dictsz;
    data_t wordreposz;

    data_t dscap;
    data_t rscap;
    data_t dictcap;
    data_t heapcap;
    data_t codecap;
    data_t linenum;
    data_t last_created;

    bool ready;
    bool finished;
    FILE *in;
    FILE *out;
    char *curword;
    char *errmsg;
};
typedef struct forthvm forthvm_t;

data_t vm_pop_ds(forthvm_t *vm);
void vm_push_ds(forthvm_t *vm, data_t d);
data_t vm_pop_rs(forthvm_t *vm);
void vm_push_rs(forthvm_t *vm, data_t d);
data_t vm_read_word(forthvm_t *vm);
void vm_emit_data(forthvm_t *vm, data_t d);
void vm_emit_opcode(forthvm_t *vm, opcode_t);
void vm_heapsz(forthvm_t *vm, data_t size);
void vm_heap_grow(forthvm_t *vm, data_t size);
char vm_getc(forthvm_t *vm);
void vm_ungetc(forthvm_t *vm, char c);
void vm_regfunc(forthvm_t *vm, char *word, opfunc f);

data_t vm_execute(forthvm_t *vm);
void vm_init(forthvm_t *vm, FILE *fin, FILE *fout);
void vm_run(forthvm_t *vm);

#endif
