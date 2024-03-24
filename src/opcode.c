#include "opcode.h"

#include <assert.h>
#include <string.h>

#include "vm.h"

#define CHECKERR                                                               \
    if (vm->finished)                                                          \
        return;

#define CHECKDS(len)                                                           \
    if (vm->dsp < len) {                                                       \
        vm->ret = -1;                                                          \
        vm->finished = true;                                                   \
        vm->errmsg = "no enough element on data_t stack";                      \
        return;                                                                \
    }

#define CHECKRS(len)                                                           \
    if (vm->rsp < len) {                                                       \
        vm->ret = -1;                                                          \
        vm->finished = true;                                                   \
        vm->errmsg = "no enough element on return stack";                      \
        return;                                                                \
    }

char *op_vec[OP_NOP + 1] = {
    [OP_DUMP] = "dump",
    [OP_RDUMP] = "rdump",
    [OP_DEPTH] = "depth",
    [OP_EMIT] = "emit",
    [OP_ASSERT] = "assert",
    [OP_ROT] = "rot",
    [OP_PRINT] = "print",
    [OP_COMMA] = ",",
    [OP_HERE] = "here",
    [OP_CR] = "cr",
    [OP_ADD] = "+",
    [OP_MINUS] = "-",
    [OP_MUL] = "*",
    [OP_DIV] = "/",
    [OP_MOD] = "mod",
    [OP_DIVMOD] = "/mod",
    [OP_MIN] = "min",
    [OP_MAX] = "max",
    [OP_NEGATE] = "negate",
    [OP_EQ] = "=",
    [OP_NEQ] = "<>",
    [OP_GT] = ">",
    [OP_LT] = "<",
    [OP_GE] = ">=",
    [OP_LE] = "<=",
    [OP_AND] = "and",
    [OP_OR] = "or",
    [OP_OR] = "or",
    [OP_NOT] = "not",
    [OP_BITAND] = "bitand",
    [OP_BITOR] = "bitor",
    [OP_INVERT] = "invert",
    [OP_XOR] = "xor",
    [OP_DUP] = "dup",
    [OP_OVER] = "over",
    [OP_SWAP] = "swap",
    [OP_DROP] = "drop",
    [OP_DOT] = ".",
    [OP_CALL] = "call\t",
    [OP_PUSH] = "push\t",
    [OP_CREATE] = "create",
    [OP_BYE] = "bye",
    [OP_EXIT] = "exit",
    [OP_JMP] = "jmp\t",
    [OP_JZ] = "jz\t",
    [OP_CELLS] = "cells",
    [OP_CHARS] = "chars",
    [OP_ALLOT] = "allot",
    [OP_ALLOCATE] = "allocate",
    [OP_RESIZE] = "resize",
    [OP_FREE] = "free",
    [OP_BANG] = "!",
    [OP_AT] = "@",
    [OP_NOP] = "nop\t",
    [OP_PICK] = "pick",
    [OP_RPICK] = "rpick",
    [OP_R2D] = "r>",
    [OP_D2R] = ">r",
    [OP_RAT] = "r@",
    [OP_EXECUTE] = "execute",
    [OP_QUOTE] = "'",
    [OP_CFUNC] = "cfunc\t",
    [OP_DO] = "do\t",
    [OP_LOOP] = "loop\t",
    [OP_PLUSLOOP] = "+loop\t",
    [OP_CFUNC] = "cfunc\t",
    [OP_I] = "i",
    [OP_II] = "i'",
    [OP_J] = "j",
    [OP_HEAPSIZE] = "heap-size",
    [OP_DOES] = "does>",
};

opfunc op_funcvec[OP_NOP + 1] = {
    [OP_I] = op_i,
    [OP_II] = op_ii,
    [OP_J] = op_j,
    [OP_DO] = op_do,
    [OP_LOOP] = op_loop,
    [OP_PLUSLOOP] = op_plusloop,
    [OP_EXECUTE] = op_execute,
    [OP_QUOTE] = op_quote,
    [OP_CFUNC] = op_cfunc,
    [OP_PICK] = op_pick,
    [OP_RPICK] = op_rpick,
    [OP_R2D] = op_r2d,
    [OP_D2R] = op_d2r,
    [OP_RAT] = op_rat,
    [OP_DUMP] = op_dump,
    [OP_RDUMP] = op_rdump,
    [OP_DEPTH] = op_depth,
    [OP_ASSERT] = op_assert,
    [OP_EMIT] = op_emit,
    [OP_ROT] = op_rot,
    [OP_PRINT] = op_print,
    [OP_COMMA] = op_comma,
    [OP_HERE] = op_here,
    [OP_CR] = op_cr,
    [OP_ADD] = op_add,
    [OP_MINUS] = op_minus,
    [OP_MUL] = op_mul,
    [OP_DIV] = op_div,
    [OP_MOD] = op_mod,
    [OP_DIVMOD] = op_divmod,
    [OP_MIN] = op_min,
    [OP_MAX] = op_max,
    [OP_NEGATE] = op_negate,
    [OP_EQ] = op_eq,
    [OP_NEQ] = op_neq,
    [OP_GT] = op_gt,
    [OP_LT] = op_lt,
    [OP_GE] = op_ge,
    [OP_LE] = op_le,
    [OP_AND] = op_and,
    [OP_OR] = op_or,
    [OP_NOT] = op_not,
    [OP_BITAND] = op_bitand,
    [OP_BITOR] = op_bitor,
    [OP_INVERT] = op_invert,
    [OP_XOR] = op_xor,
    [OP_DUP] = op_dup,
    [OP_OVER] = op_over,
    [OP_SWAP] = op_swap,
    [OP_DROP] = op_drop,
    [OP_DOT] = op_dot,
    [OP_CALL] = op_call,
    [OP_PUSH] = op_push,
    [OP_CREATE] = op_create,
    [OP_BYE] = op_bye,
    [OP_EXIT] = op_exit,
    [OP_JMP] = op_jmp,
    [OP_JZ] = op_jz,
    [OP_CELLS] = op_cells,
    [OP_CHARS] = op_chars,
    [OP_ALLOT] = op_allot,
    [OP_ALLOCATE] = op_allocate,
    [OP_RESIZE] = op_resize,
    [OP_FREE] = op_free,
    [OP_BANG] = op_bang,
    [OP_AT] = op_at,
    [OP_NOP] = op_nop,
    [OP_HEAPSIZE] = op_heapsize,
    [OP_DOES] = op_does,
};

char *get_opname(opcode_t op) { return op_vec[(int)op]; }

opfunc get_opfunc(opcode_t op) { return op_funcvec[(int)op]; }

data_t get_opaddr(opcode_t op) {
    opfunc f = op_funcvec[(int)op];
    return *(data_t *)&f;
}

void op_heapsize(forthvm_t *vm) {
    data_t sz = vm_pop_ds(vm);
    CHECKERR;
    vm_heapsz(vm, sz);
}

void op_i(forthvm_t *vm) {
    CHECKRS(1);
    vm_push_ds(vm, vm->rs[vm->rsp]);
}

void op_ii(forthvm_t *vm) {
    CHECKRS(2);
    vm_push_ds(vm, vm->rs[vm->rsp - 1]);
}

void op_j(forthvm_t *vm) {
    CHECKRS(3);
    vm_push_ds(vm, vm->rs[vm->rsp - 2]);
}

void op_do(forthvm_t *vm) {
    CHECKRS(2);
    data_t i = vm->rs[vm->rsp];
    data_t limit = vm->rs[vm->rsp - 1];
    vm->pc++;
    data_t end = vm->code[vm->pc];
    if (i < limit) {
        return;
    }
    vm->pc = end - 1;
}

void op_loop(forthvm_t *vm) { vm->rs[vm->rsp]++; }

void op_plusloop(forthvm_t *vm) {
    data_t inc = vm_pop_ds(vm);
    CHECKERR;
    CHECKRS(1);
    vm->rs[vm->rsp] += inc;
}

void op_pick(forthvm_t *vm) {
    data_t a = vm_pop_ds(vm);
    CHECKERR;
    CHECKDS(a);
    vm_push_ds(vm, vm->ds[vm->dsp - a]);
}

void op_rpick(forthvm_t *vm) {
    data_t a = vm_pop_ds(vm);
    CHECKERR;
    CHECKRS(a);
    vm_push_ds(vm, vm->rs[vm->rsp - a]);
}

void op_d2r(forthvm_t *vm) {
    data_t a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_rs(vm, a);
}

void op_r2d(forthvm_t *vm) {
    data_t a = vm_pop_rs(vm);
    CHECKERR;
    vm_push_ds(vm, a);
}

void op_rat(forthvm_t *vm) {
    CHECKRS(1);
    vm_push_ds(vm, vm->rs[vm->rsp]);
}

void op_dump(forthvm_t *vm) {
    data_t depth = vm->dsp;
    fprintf(vm->out, "<%lld> ", depth);
    for (int i = 1; i <= depth; i++) {
        fprintf(vm->out, "%ld ", vm->ds[i]);
    }
    fprintf(vm->out, "<top>");
}

void op_rdump(forthvm_t *vm) {
    data_t depth = vm->rsp;
    fprintf(vm->out, "[%lld] ", depth);
    for (int i = 1; i <= depth; i++) {
        fprintf(vm->out, "%ld ", vm->rs[i]);
    }
    fprintf(vm->out, "[top]");
}

void op_depth(forthvm_t *vm) {
    data_t depth = vm->dsp;
    vm_push_ds(vm, depth);
}

void op_emit(forthvm_t *vm) {
    char c = vm_pop_ds(vm);
    CHECKERR;
    fprintf(vm->out, "%c", c);
}

void op_assert(forthvm_t *vm) {
    data_t a = vm_pop_ds(vm);
    CHECKERR;
    if (!a) {
        vm->finished = true;
        vm->ret = -2;
    }
}

void op_rot(forthvm_t *vm) {
    CHECKDS(3);
    data_t t = vm->ds[vm->dsp - 2];
    vm->ds[vm->dsp - 2] = vm->ds[vm->dsp - 1];
    vm->ds[vm->dsp - 1] = vm->ds[vm->dsp];
    vm->ds[vm->dsp] = t;
}

void op_comma(forthvm_t *vm) {
    vm_heap_grow(vm, sizeof(data_t));
    CHECKERR;
    data_t *p = vm->heaptop - sizeof(data_t);
    data_t a = vm_pop_ds(vm);
    CHECKERR;
    *p = a;
}

void op_print(forthvm_t *vm) {
    char *s = (char *)vm_pop_ds(vm);
    CHECKERR;
    fprintf(vm->out, "%s", s);
}

void op_here(forthvm_t *vm) {
    data_t a = (data_t)vm->heaptop;
    vm_push_ds(vm, a);
}

void op_cr(forthvm_t *vm) { fprintf(vm->out, "\n"); }

void op_add(forthvm_t *vm) {
    data_t a, b;
    a = vm_pop_ds(vm);
    CHECKERR;
    b = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a + b);
}

void op_minus(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a - b);
}

void op_mul(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a * b);
}

void op_div(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a / b);
}

void op_mod(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a % b);
}

void op_divmod(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a % b);
    vm_push_ds(vm, a / b);
}

void op_min(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a < b ? a : b);
}

void op_max(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a > b ? a : b);
}

void op_negate(forthvm_t *vm) {
    data_t a;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, -a);
}

void op_eq(forthvm_t *vm) {
    data_t a, b;
    a = vm_pop_ds(vm);
    CHECKERR;
    b = vm_pop_ds(vm);
    CHECKERR;
    if (a == b)
        vm_push_ds(vm, -1);
    else
        vm_push_ds(vm, 0);
}

void op_neq(forthvm_t *vm) {
    data_t a, b;
    a = vm_pop_ds(vm);
    CHECKERR;
    b = vm_pop_ds(vm);
    CHECKERR;
    if (a == b)
        vm_push_ds(vm, 0);
    else
        vm_push_ds(vm, -1);
}

void op_gt(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    data_t r;
    if (a > b) {
        r = -1;
    } else {
        r = 0;
    }
    vm_push_ds(vm, r);
}

void op_lt(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    data_t r;
    if (a < b) {
        r = -1;
    } else {
        r = 0;
    }
    vm_push_ds(vm, r);
}

void op_ge(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    data_t r;
    if (a >= b) {
        r = -1;
    } else {
        r = 0;
    }
    vm_push_ds(vm, r);
}

void op_le(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    data_t r;
    if (a <= b) {
        r = -1;
    } else {
        r = 0;
    }
    vm_push_ds(vm, r);
}

void op_and(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a && b ? -1 : 0);
}

void op_or(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a || b ? -1 : 0);
}

void op_not(forthvm_t *vm) {
    data_t a;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a ? 0 : -1);
}

void op_bitand(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a & b);
}

void op_bitor(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a | b);
}

void op_invert(forthvm_t *vm) {
    data_t a;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, ~a);
}

void op_xor(forthvm_t *vm) {
    data_t a, b;
    b = vm_pop_ds(vm);
    CHECKERR;
    a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a ^ b);
}

void op_dup(forthvm_t *vm) {
    CHECKDS(1);
    data_t a;
    a = vm->ds[vm->dsp];
    vm_push_ds(vm, a);
}

void op_over(forthvm_t *vm) {
    CHECKDS(2);
    data_t a = vm->ds[vm->dsp - 1];
    vm_push_ds(vm, a);
}

void op_swap(forthvm_t *vm) {
    CHECKDS(2);
    data_t t;
    t = vm->ds[vm->dsp];
    vm->ds[vm->dsp] = vm->ds[vm->dsp - 1];
    vm->ds[vm->dsp - 1] = t;
}

void op_drop(forthvm_t *vm) {
    CHECKDS(1);
    vm->dsp--;
}

void op_dot(forthvm_t *vm) {
    data_t a = vm_pop_ds(vm);
    CHECKERR;
    fprintf(vm->out, "%ld ", a);
}

void op_push(forthvm_t *vm) {
    vm->pc++;
    data_t a = vm->code[vm->pc];
    vm_push_ds(vm, a);
}

void op_bye(forthvm_t *vm) {
    vm->finished = true;
    vm->ret = 0;
}

void op_quote(forthvm_t *vm) {
    data_t a = vm_read_word(vm);
    vm_push_ds(vm, a);
}

void op_create(forthvm_t *vm) {
    vm_emit_opcode(vm, OP_JMP);
    vm_emit_data(vm, 0);
    data_t addr_ptr = vm->codesz - 1;
    data_t a = vm_read_word(vm);
    data_t b = vm->codesz;
    vm->dict[a] = b;
    vm_emit_opcode(vm, OP_PUSH);
    vm_emit_data(vm, (data_t)vm->heaptop);
    vm_emit_opcode(vm, OP_EXIT);
    vm->last_created = vm->codesz - 1;
    vm_emit_opcode(vm, OP_NOP);
    vm->code[addr_ptr] = vm->codesz;
}

void op_does(forthvm_t *vm) {
    if (vm->last_created < 0) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "create and does> not paired.";
        return;
    }
    data_t lc = vm->last_created;
    vm->code[lc] = get_opaddr(OP_JMP);
    vm->code[lc + 1] = vm->pc + 1;
    vm->last_created = -1;
    op_exit(vm);
}

void op_jmp(forthvm_t *vm) {
    vm->pc++;
    data_t addr = vm->code[vm->pc];
    if (addr < 0) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "failed to jmp, invalid address";
    }
    vm->pc = addr - 1;
}

void op_jz(forthvm_t *vm) {
    vm->pc++;
    data_t addr = vm->code[vm->pc];
    if (addr < 0) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "failed to jz, invalid address";
    }
    data_t d = vm_pop_ds(vm);
    CHECKERR;
    if (d == 0)
        vm->pc = addr - 1;
}

void op_exit(forthvm_t *vm) {
    data_t addr = vm_pop_rs(vm);
    vm->pc = addr;
}

void op_execute(forthvm_t *vm) {
    data_t entry = vm_pop_ds(vm);
    CHECKERR;
    if (entry <= OP_NOP) {
        opfunc f = get_opfunc(entry);
        (*f)(vm);
        return;
    }
    data_t addr = vm->dict[entry];
    if (addr < 0) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "undefined word";
    }
    vm_push_rs(vm, vm->pc);
    vm->pc = addr - 1;
}

void op_cfunc(forthvm_t *vm) {
    vm->pc++;
    data_t addr = vm->code[vm->pc];
    opfunc f = *(opfunc *)&addr;
    (*f)(vm);
}

void op_call(forthvm_t *vm) {
    vm->pc++;
    data_t entry = vm->code[vm->pc];
    data_t addr = vm->dict[entry];
    if (addr < 0) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "undefined word";
    }
    vm_push_rs(vm, vm->pc);
    vm->pc = addr - 1;
}

void op_cells(forthvm_t *vm) {
    data_t a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a * sizeof(data_t));
}

void op_chars(forthvm_t *vm) {
    data_t a = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, a * sizeof(char));
}

void op_allot(forthvm_t *vm) {
    data_t size = vm_pop_ds(vm);
    CHECKERR;
    vm_heap_grow(vm, size);
    CHECKERR;
}

void op_allocate(forthvm_t *vm) {
    data_t size = vm_pop_ds(vm);
    CHECKERR;
    void *buf = malloc(size);
    vm_push_ds(vm, (data_t)buf);
    CHECKERR;
}

void op_resize(forthvm_t *vm) {
    CHECKDS(2);
    data_t size = vm_pop_ds(vm);
    data_t addr = vm_pop_ds(vm);
    void *buf = (void *)addr;
    buf = realloc(buf, size);
    vm_push_ds(vm, (data_t)buf);
}

void op_free(forthvm_t *vm) {
    data_t addr = vm_pop_ds(vm);
    CHECKERR;
    void *buf = (void *)addr;
    free(buf);
}

void op_bang(forthvm_t *vm) {
    data_t addr = vm_pop_ds(vm);
    CHECKERR;
    data_t x = vm_pop_ds(vm);
    CHECKERR;
    *(data_t *)addr = x;
}

void op_at(forthvm_t *vm) {
    data_t addr = vm_pop_ds(vm);
    CHECKERR;
    vm_push_ds(vm, *(data_t *)addr);
}

void op_nop(forthvm_t *vm) {}
