#include "vm.h"

#include <string.h>

#include "token.h"

static void *make_space(void *buf, data_t *cap, data_t idx) {
    if (*cap <= idx) {
        void *newbuf = realloc(buf, sizeof(data_t) * idx * 2);
        *cap = idx * 2;
        return newbuf;
    }
    return buf;
}

static data_t create_word(forthvm_t *vm, char *word) {
    char *dup_word = strdup(word);
    vm->dict = make_space(vm->dict, &vm->dictcap, vm->dictsz);
    vm->dict[vm->dictsz] = -1;
    word_table_append(vm->wordtable, dup_word, vm->dictsz);
    vm->dictsz++;
    return vm->dictsz - 1;
}

static data_t find_word(forthvm_t *vm, char *word) {
    data_t entry = word_table_find(vm->wordtable, word);
    if (entry >= 0)
        return entry;
    return create_word(vm, word);
}

data_t vm_pop_ds(forthvm_t *vm) {
    if (vm->dsp <= 0) {
        vm->errmsg = "pop from data_t stack failed";
        vm->finished = true;
        vm->ret = -1;
        return -1;
    }
    data_t r = vm->ds[vm->dsp];
    vm->dsp--;
    return r;
}

void vm_push_ds(forthvm_t *vm, data_t d) {
    vm->dsp++;
    vm->ds = make_space(vm->ds, &vm->dscap, vm->dsp);
    vm->ds[vm->dsp] = d;
}

data_t vm_pop_rs(forthvm_t *vm) {
    if (vm->rsp <= 0) {
        vm->errmsg = "pop from return stack failed";
        vm->finished = true;
        vm->ret = -1;
        return -1;
    }
    data_t r = vm->rs[vm->rsp];
    vm->rsp--;
    return r;
}

void vm_push_rs(forthvm_t *vm, data_t d) {
    vm->rsp++;
    vm->rs = make_space(vm->rs, &vm->rscap, vm->rsp);
    vm->rs[vm->rsp] = d;
}

void vm_emit_data(forthvm_t *vm, data_t d) {
    vm->code = make_space(vm->code, &vm->codecap, vm->codesz);
    vm->code[vm->codesz] = d;
    vm->codesz++;
}

void vm_emit_opcode(forthvm_t *vm, opcode_t op) {
    data_t d = get_opaddr(op);
    vm_emit_data(vm, d);
}

void vm_init(forthvm_t *vm, FILE *fin, FILE *fout) {
    *vm = (forthvm_t){0};

    vm->ds = malloc(1024 * sizeof(data_t));
    vm->rs = malloc(1024 * sizeof(data_t));
    vm->heap = malloc(4096);
    vm->dict = malloc(1024 * sizeof(data_t));
    vm->code = malloc(1024 * sizeof(data_t));
    vm->heaptop = vm->heap;

    vm->dscap = 1024;
    vm->rscap = 1024;
    vm->heapcap = 4096;
    vm->dictcap = 1024;
    vm->codecap = 1024;
    vm->linenum = 1;
    vm->last_created = -1;

    vm->curword = malloc(1024);
    vm->wordtable = malloc(sizeof(word_table_t));
    word_table_init(vm->wordtable);
    vm->ready = true;
    vm->errmsg = "";

    for (data_t i = 0; i < (data_t)OP_NOP + 1; i++) {
        find_word(vm, get_opname((opcode_t)i));
    }

    vm->in = fin;
    vm->out = fout;
}

void vm_heapsz(forthvm_t *vm, data_t sz) {
    vm->heap = realloc(vm->heap, sz);
    vm->heaptop = vm->heap;
    vm->heapcap = sz;
}

data_t vm_execute(forthvm_t *vm) {
    if (!vm->ready)
        return 0;
    data_t a, b;
    opfunc func_ptr;
    data_t ret = 0;
    opcode_t op;

    while (!vm->finished) {
        if (vm->pc >= vm->codesz) {
            break;
        }
        data_t op_addr = vm->code[vm->pc];
        opfunc opf = *(opfunc *)&op_addr;
        (*opf)(vm);
        vm->pc++;
    }
    return ret;
}

static int compile(forthvm_t *vm) {
    token_t tok;
    opfunc fn;
    data_t entry;
    while (!vm->finished) {
        tok = get_token(vm);
        switch (tok.type) {
        case TOK_NUM:
            vm_emit_opcode(vm, OP_PUSH);
            vm_emit_data(vm, tok.dat);
            break;
        case TOK_WORD:
            entry = find_word(vm, vm->curword);
            if (entry < (data_t)OP_NOP) {
                vm_emit_opcode(vm, entry);
            } else {
                vm_emit_opcode(vm, OP_CALL);
                vm_emit_data(vm, entry);
            }
            if (vm->ready)
                return 1;
            break;
        case TOK_SYNTAX:
            fn = get_syntax_op(tok.dat);
            (*fn)(vm);
            break;
        case TOK_EOF:
            vm_execute(vm);
            vm->finished = true;
            break;
        default:
            break;
        }
    }
    return 0;
}

void vm_heap_grow(forthvm_t *vm, data_t size) {
    if (vm->heaptop + size - vm->heap <= vm->heapcap) {
        vm->heaptop += size;
        return;
    }
    vm->finished = true;
    vm->ret = -1;
    vm->errmsg = "failed to allot memory";
}

data_t vm_read_word(forthvm_t *vm) {
    token_t tok;
    tok = get_token(vm);
    if (tok.type != TOK_WORD) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "next input token is expeted to be a word";
        return -1;
    }
    return find_word(vm, vm->curword);
}

char vm_getc(forthvm_t *vm) {
    char c = fgetc(vm->in);
    if (c == '\n')
        vm->linenum++;
    return c;
}

void vm_ungetc(forthvm_t *vm, char c) {
    if (c == EOF)
        return;
    if (c == '\n')
        vm->linenum--;
    ungetc(c, vm->in);
}

void vm_regfunc(forthvm_t *vm, char *word, opfunc f) {
    data_t entry = find_word(vm, word);
    data_t faddr = *(data_t *)&f;
    vm_emit_opcode(vm, OP_JMP);
    vm_emit_data(vm, vm->codesz + 4);
    vm->dict[entry] = vm->codesz;
    vm_emit_opcode(vm, OP_CFUNC);
    vm_emit_data(vm, faddr);
    vm_emit_opcode(vm, OP_EXIT);
}

void vm_run(forthvm_t *vm) {
    data_t ret;
    while (!vm->finished) {
        while (!vm->finished && !compile(vm))
            ;
        vm_execute(vm);
    }
}
