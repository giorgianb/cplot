#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "../include/parser.h"

void expression_print(const expression_t expression);

int main(void) {
  for (;;) {
    expression_t e = next_expression(stdin);
    expression_print(e);
    putchar('\n');
    expression_destroy(e);
  }

  return 0;
}

void expression_print(const expression_t expression) {
  switch (expression.type) {
    case EXPRESSION_FUNCTION:
      printf("(%s ", expression.s);
      expression_print(expression.operands[0]);
      putchar(')');
      break;
    case EXPRESSION_OPERATOR:
      printf("(%c ", expression.operator);
      expression_print(expression.operands[0]);
      if (expression.operator != 'N') {
        putchar(' ');
        expression_print(expression.operands[1]);
      }
      putchar(')');
      break;
    case EXPRESSION_NUMBER:
      printf("%lf", expression.d);
      break;
    case EXPRESSION_ERROR:
      printf("ERROR");
      break;
    case EXPRESSION_VARIABLE:
      printf("%s", expression.s);
      break;
  }
}


