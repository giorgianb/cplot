#ifndef __EXPRESSION_INC
#define __EXPRESSION_INC

enum expression_type {
  EXPRESSION_FUNCTION, EXPRESSION_OPERATOR, EXPRESSION_NUMBER, EXPRESSION_ERROR, EXPRESSION_VARIABLE
};

struct expression {
  enum expression_type type;

  union {
    double d;
    struct expression *operands;
  };

  union {
    char operator;
    char *s;
  };
};

typedef struct expression expression_t;

expression_t next_expression(FILE *const stream);
void expression_destroy(const expression_t expression);
#endif
