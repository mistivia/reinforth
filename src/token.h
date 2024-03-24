#ifndef REINFORTH_TOKEN_H_
#define REINFORTH_TOKEN_H_

#include <stdio.h>

#include "types.h"

enum token_type {
    TOK_NUM,
    TOK_WORD,
    TOK_SYNTAX,
    TOK_EOF,
    TOK_INVALID,
};
typedef enum token_type token_type_t;

struct token {
    token_type_t type;
    data_t dat;
};
typedef struct token token_t;

struct forthvm;
typedef struct forthvm forthvm_t;

token_t get_token(forthvm_t *vm);

#endif
