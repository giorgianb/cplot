#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "tokenizer.h"

int main(void) {
  for (token_t tok = next_token(stdin); tok.type != TOKEN_END; tok = next_token(stdin)) {
    switch (tok.type) {
      case TOKEN_STRING:
        printf("string: %s\n", tok.s);
        perror("");
        free(tok.s);
        break;
      case TOKEN_FUNCTION:
        printf("function: %s\n", tok.s);
        perror("");
        free(tok.s);
        break;
      case TOKEN_NUMBER:
        printf("number: %lf\n", tok.d);
        perror("");
        break;
      case TOKEN_ARITHMETIC_OPERATOR:
        printf("operator: %c\n", tok.operator);
        perror("");
        break;
      default:
        puts("Unknown");
        break;
    }
  }

  return 0;
}
