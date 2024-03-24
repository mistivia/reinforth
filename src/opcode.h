#ifndef REINFORTH_OPCODE_H_
#define REINFORTH_OPCODE_H_

#include "types.h"

enum opcode {
    OP_ADD,
    OP_MINUS,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_DIVMOD,
    OP_MIN,
    OP_MAX,
    OP_NEGATE,
    OP_EQ,
    OP_NEQ,
    OP_GT,
    OP_LT,
    OP_GE,
    OP_LE,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_BITAND,
    OP_BITOR,
    OP_INVERT,
    OP_XOR,
    OP_DUP,
    OP_OVER,
    OP_SWAP,
    OP_DROP,
    OP_DOT,
    OP_CALL,
    OP_PUSH,
    OP_CREATE,
    OP_BYE,
    OP_EXIT,
    OP_JMP,
    OP_JZ,
    OP_CELLS,
    OP_CHARS,
    OP_ALLOT,
    OP_ALLOCATE,
    OP_RESIZE,
    OP_FREE,
    OP_BANG,
    OP_AT,
    OP_COMMA,
    OP_HERE,
    OP_CR,
    OP_PRINT,
    OP_EMIT,
    OP_ASSERT,
    OP_ROT,
    OP_DUMP,
    OP_RDUMP,
    OP_DEPTH,
    OP_PICK,
    OP_RPICK,
    OP_D2R,
    OP_R2D,
    OP_RAT,
    OP_EXECUTE,
    OP_QUOTE,
    OP_CFUNC,
    OP_DO,
    OP_LOOP,
    OP_PLUSLOOP,
    OP_I,
    OP_II,
    OP_J,
    OP_HEAPSIZE,
    OP_DOES,
    OP_NOP,
};
typedef enum opcode opcode_t;

struct forthvm;
typedef struct forthvm forthvm_t;

void op_i(forthvm_t *vm);
void op_ii(forthvm_t *vm);
void op_j(forthvm_t *vm);
void op_do(forthvm_t *vm);
void op_loop(forthvm_t *vm);
void op_plusloop(forthvm_t *vm);
void op_execute(forthvm_t *vm);
void op_quote(forthvm_t *vm);
void op_cfunc(forthvm_t *vm);
void op_pick(forthvm_t *vm);
void op_rpick(forthvm_t *vm);
void op_r2d(forthvm_t *vm);
void op_d2r(forthvm_t *vm);
void op_rat(forthvm_t *vm);
void op_dump(forthvm_t *vm);
void op_rdump(forthvm_t *vm);
void op_depth(forthvm_t *vm);
void op_emit(forthvm_t *vm);
void op_assert(forthvm_t *vm);
void op_rot(forthvm_t *vm);
void op_cr(forthvm_t *vm);
void op_comma(forthvm_t *vm);
void op_here(forthvm_t *vm);
void op_add(forthvm_t *vm);
void op_minus(forthvm_t *vm);
void op_mul(forthvm_t *vm);
void op_div(forthvm_t *vm);
void op_mod(forthvm_t *vm);
void op_divmod(forthvm_t *vm);
void op_min(forthvm_t *vm);
void op_max(forthvm_t *vm);
void op_negate(forthvm_t *vm);
void op_eq(forthvm_t *vm);
void op_neq(forthvm_t *vm);
void op_gt(forthvm_t *vm);
void op_lt(forthvm_t *vm);
void op_ge(forthvm_t *vm);
void op_le(forthvm_t *vm);
void op_and(forthvm_t *vm);
void op_or(forthvm_t *vm);
void op_not(forthvm_t *vm);
void op_bitand(forthvm_t *vm);
void op_bitor(forthvm_t *vm);
void op_invert(forthvm_t *vm);
void op_xor(forthvm_t *vm);
void op_dup(forthvm_t *vm);
void op_over(forthvm_t *vm);
void op_swap(forthvm_t *vm);
void op_drop(forthvm_t *vm);
void op_dot(forthvm_t *vm);
void op_call(forthvm_t *vm);
void op_push(forthvm_t *vm);
void op_create(forthvm_t *vm);
void op_bye(forthvm_t *vm);
void op_exit(forthvm_t *vm);
void op_jmp(forthvm_t *vm);
void op_jz(forthvm_t *vm);
void op_cells(forthvm_t *vm);
void op_chars(forthvm_t *vm);
void op_allot(forthvm_t *vm);
void op_allocate(forthvm_t *vm);
void op_resize(forthvm_t *vm);
void op_free(forthvm_t *vm);
void op_bang(forthvm_t *vm);
void op_at(forthvm_t *vm);
void op_print(forthvm_t *vm);
void op_heapsize(forthvm_t *vm);
void op_does(forthvm_t *vm);
void op_nop(forthvm_t *vm);

char *get_opname(opcode_t);

opfunc get_opfunc(opcode_t op);
data_t get_opaddr(opcode_t op);

#endif
