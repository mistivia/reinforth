#ifndef REINFORTH_SYNTAX_H_
#define REINFORTH_SYNTAX_H_

#include "types.h"

enum syntax {
    SYN_COLON,
    SYN_SEMI,
    SYN_BEGIN,
    SYN_AGAIN,
    SYN_UNTIL,
    SYN_WHILE,
    SYN_REPEAT,
    SYN_IF,
    SYN_ELSE,
    SYN_THEN,
    SYN_DO,
    SYN_LEAVE,
    SYN_LOOP,
    SYN_PLUSLOOP,
    SYN_NOP,
};
typedef enum syntax syntax_t;

int get_syntax(char *word);
opfunc get_syntax_op(syntax_t);

void syn_colon(forthvm_t *vm);
void syn_semi(forthvm_t *vm);
void syn_begin(forthvm_t *vm);
void syn_until(forthvm_t *vm);
void syn_again(forthvm_t *vm);
void syn_while(forthvm_t *vm);
void syn_repeat(forthvm_t *vm);
void syn_if(forthvm_t *vm);
void syn_else(forthvm_t *vm);
void syn_then(forthvm_t *vm);
void syn_do(forthvm_t *vm);
void syn_leave(forthvm_t *vm);
void syn_loop(forthvm_t *vm);
void syn_plusloop(forthvm_t *vm);
void syn_nop(forthvm_t *vm);

#endif
