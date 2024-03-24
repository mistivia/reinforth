#include "syntax.h"

#include <string.h>

#include "opcode.h"
#include "vm.h"

#define CHECKCOMPILE                                                           \
    if (vm->ready) {                                                           \
        vm->finished = true;                                                   \
        vm->ret = -1;                                                          \
        vm->errmsg = "invalid word during interpreting";                       \
        return;                                                                \
    }

char *syntax_name[SYN_NOP] = {
    [SYN_COLON] = ":",     [SYN_SEMI] = ";",         [SYN_BEGIN] = "begin",
    [SYN_UNTIL] = "until", [SYN_IF] = "if",          [SYN_ELSE] = "else",
    [SYN_THEN] = "then",   [SYN_DO] = "do",          [SYN_LEAVE] = "leave",
    [SYN_LOOP] = "loop",   [SYN_PLUSLOOP] = "+loop", [SYN_AGAIN] = "again",
    [SYN_WHILE] = "while", [SYN_REPEAT] = "repeat",
};

opfunc syntax_ops[SYN_NOP + 1] = {
    [SYN_WHILE] = syn_while,       [SYN_AGAIN] = syn_again,
    [SYN_REPEAT] = syn_repeat,     [SYN_COLON] = syn_colon,
    [SYN_SEMI] = syn_semi,         [SYN_BEGIN] = syn_begin,
    [SYN_UNTIL] = syn_until,       [SYN_IF] = syn_if,
    [SYN_ELSE] = syn_else,         [SYN_THEN] = syn_then,
    [SYN_NOP] = syn_nop,           [SYN_DO] = syn_do,
    [SYN_LEAVE] = syn_leave,       [SYN_LOOP] = syn_loop,
    [SYN_PLUSLOOP] = syn_plusloop,
};

int get_syntax(char *word) {
    for (int i = 0; i < SYN_NOP; i++) {
        if (strcmp(word, syntax_name[i]) == 0) {
            return i;
        }
    }
    return -1;
}

opfunc get_syntax_op(syntax_t s) {
    if (s > SYN_NOP)
        return syn_nop;
    return syntax_ops[(int)s];
}

void syn_nop(forthvm_t *vm) {}

void syn_colon(forthvm_t *vm) {
    vm_execute(vm);
    if (vm->rsp > 0) {
        vm->errmsg = "wrong place to start word definition";
        vm->finished = true;
        vm->ret = -1;
        return;
    }
    data_t entry = vm_read_word(vm);
    vm->dict[entry] = vm->codesz;
    vm_push_rs(vm, SYN_COLON);
    vm->ready = false;
}

void syn_semi(forthvm_t *vm) {
    syntax_t s = vm_pop_rs(vm);
    if (vm->finished)
        return;
    if (s != SYN_COLON) {
        vm->errmsg = "expect semicolon";
        vm->finished = true;
        vm->ret = -1;
        return;
    }
    vm_emit_opcode(vm, OP_EXIT);
    vm->pc = vm->codesz;
    vm->ready = true;
}

void syn_if(forthvm_t *vm) {
    vm_emit_opcode(vm, OP_JZ);
    vm_push_rs(vm, vm->codesz);
    vm_emit_data(vm, -1);
    vm_push_rs(vm, SYN_IF);
}

void syn_else(forthvm_t *vm) {
    data_t d = vm_pop_rs(vm);
    if (d != SYN_IF || vm->finished) {
        vm->errmsg = "unexpected else";
        vm->finished = true;
        vm->ret = -1;
        return;
    }
    d = vm_pop_rs(vm);
    vm_emit_opcode(vm, OP_JMP);
    vm_push_rs(vm, vm->codesz);
    vm_push_rs(vm, SYN_ELSE);
    vm_emit_opcode(vm, -1);
    vm->code[d] = vm->codesz;
}

void syn_do(forthvm_t *vm) {
    CHECKCOMPILE;
    vm_emit_opcode(vm, OP_SWAP);
    vm_emit_opcode(vm, OP_D2R);
    vm_emit_opcode(vm, OP_D2R);
    vm_emit_opcode(vm, OP_DO);
    vm_push_rs(vm, vm->codesz);
    vm_emit_data(vm, -1);
    vm_push_rs(vm, SYN_DO);
}

void syn_leave(forthvm_t *vm) {
    CHECKCOMPILE;
    if (vm->rs[vm->rsp] != SYN_DO && vm->rs[vm->rsp] != SYN_LEAVE) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "not a do loop, cannot leave";
        return;
    }
    vm_emit_opcode(vm, OP_JMP);
    vm_push_rs(vm, vm->codesz);
    vm_emit_data(vm, -1);
    vm_push_rs(vm, SYN_LEAVE);
}

void syn_loop(forthvm_t *vm) {
    CHECKCOMPILE;
    vm_emit_opcode(vm, OP_LOOP);
    vm_emit_opcode(vm, OP_JMP);
    data_t begin_pos_ptr = vm->codesz;
    vm_emit_data(vm, -1);
    data_t end = vm->codesz;
    while (1) {
        data_t ins = vm_pop_rs(vm);
        if (ins == SYN_DO) {
            data_t begin_pos = vm_pop_rs(vm);
            vm->code[begin_pos_ptr] = begin_pos - 1;
            vm->code[begin_pos] = end;
            vm_emit_opcode(vm, OP_R2D);
            vm_emit_opcode(vm, OP_DROP);
            vm_emit_opcode(vm, OP_R2D);
            vm_emit_opcode(vm, OP_DROP);
            break;
        }
        if (ins == SYN_LEAVE) {
            vm->code[vm_pop_rs(vm)] = end;
            continue;
        } else {
            vm->finished = true;
            vm->ret = -1;
            vm->errmsg = "unexpected loop";
            return;
        }
    }
}

void syn_plusloop(forthvm_t *vm) {
    CHECKCOMPILE;
    vm_emit_opcode(vm, OP_PLUSLOOP);
    vm_emit_opcode(vm, OP_JMP);
    data_t begin_pos_ptr = vm->codesz;
    vm_emit_data(vm, -1);
    data_t end = vm->codesz;
    while (1) {
        data_t ins = vm_pop_rs(vm);
        if (ins == SYN_DO) {
            data_t begin_pos = vm_pop_rs(vm);
            vm->code[begin_pos_ptr] = begin_pos - 1;
            vm->code[begin_pos] = end;
            vm_emit_opcode(vm, OP_R2D);
            vm_emit_opcode(vm, OP_DROP);
            vm_emit_opcode(vm, OP_R2D);
            vm_emit_opcode(vm, OP_DROP);
            break;
        }
        if (ins == SYN_LEAVE) {
            vm->code[vm_pop_rs(vm)] = end;
            continue;
        } else {
            vm->finished = true;
            vm->ret = -1;
            vm->errmsg = "unexpected +loop";
            return;
        }
    }
}

void syn_begin(forthvm_t *vm) {
    CHECKCOMPILE;
    vm_push_rs(vm, vm->codesz);
    vm_push_rs(vm, SYN_BEGIN);
}

void syn_again(forthvm_t *vm) {
    CHECKCOMPILE;
    data_t syntok = vm_pop_rs(vm);
    if (syntok != SYN_BEGIN) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "syntax error, unexpected again";
        return;
    }
    data_t addr = vm_pop_rs(vm);
    vm_emit_opcode(vm, OP_JMP);
    vm_emit_data(vm, addr);
}

void syn_until(forthvm_t *vm) {
    CHECKCOMPILE;
    data_t syntok = vm_pop_rs(vm);
    if (syntok != SYN_BEGIN) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "syntax error, unexpected until";
        return;
    }
    data_t addr = vm_pop_rs(vm);
    vm_emit_opcode(vm, OP_JZ);
    vm_emit_data(vm, addr);
}

void syn_while(forthvm_t *vm) {
    CHECKCOMPILE;
    data_t syntok = vm->rs[vm->rsp];
    if (syntok != SYN_BEGIN) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "syntax error, unexpected while";
        return;
    }
    vm_emit_opcode(vm, OP_JZ);
    vm_push_rs(vm, vm->codesz);
    vm_emit_data(vm, -1);
    vm_push_rs(vm, SYN_WHILE);
}

void syn_repeat(forthvm_t *vm) {
    CHECKCOMPILE;
    data_t syntok = vm_pop_rs(vm);
    if (syntok != SYN_WHILE) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "syntax error, unexpected repeat";
        return;
    }
    data_t addr = vm_pop_rs(vm);
    vm->code[addr] = vm->codesz + 2;

    syntok = vm_pop_rs(vm);
    if (syntok != SYN_BEGIN) {
        vm->finished = true;
        vm->ret = -1;
        vm->errmsg = "syntax error, unexpected repeat";
        return;
    }
    addr = vm_pop_rs(vm);
    vm_emit_opcode(vm, OP_JMP);
    vm_emit_data(vm, addr);
}

void syn_then(forthvm_t *vm) {
    data_t d = vm_pop_rs(vm);
    if (vm->finished)
        return;
    if (d != SYN_IF && d != SYN_ELSE) {
        vm->errmsg = "expect if or else before then";
        vm->finished = true;
        vm->ret = -1;
        return;
    }
    d = vm_pop_rs(vm);
    if (vm->finished) {
        return;
    }
    vm->code[d] = vm->codesz;
}
