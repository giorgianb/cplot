#ifndef __TOKENIZER_INC
#define __TOKENIZER_INC
#include <stdio.h>

enum token_type {
  TOKEN_STRING, TOKEN_NUMBER, TOKEN_ARITHMETIC_OPERATOR, TOKEN_FUNCTION, TOKEN_END, TOKEN_UNKNOWN, TOKEN_ERROR
};

struct token {
  union {
    double d;
    char *s;
    char operator;
  };

  enum token_type type;
};

typedef struct token token_t;

token_t next_token(FILE *const stream);
void push_back_token(const token_t token);
void token_destroy(const token_t token);
#endif
