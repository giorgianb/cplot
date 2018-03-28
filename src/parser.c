#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../include/tokenizer.h"
#include "../include/parser.h"

void expression_destroy(const expression_t e) {
  switch (e.type) {
    case EXPRESSION_FUNCTION:
      free(e.s);
      expression_destroy(e.operands[0]);
      free(e.operands);
      break;
    case EXPRESSION_OPERATOR:
      expression_destroy(e.operands[0]);
      if (e.operator != 'N')
        expression_destroy(e.operands[1]);

      free(e.operands);
      break;
    case EXPRESSION_VARIABLE:
      free(e.s);
      break;
    default:
      break;
  }
}

expression_t get_nth_level_expression(FILE *const stream, const int n) {
  switch (n) {
    case 0: 
      {
        expression_t left = get_nth_level_expression(stream, n + 1);
        token_t tok = next_token(stream);
        if (tok.type != TOKEN_END) {
          expression_destroy(left);
          left.type = EXPRESSION_ERROR;
        }

        return left;
      }
    case 1: 
      {
        expression_t left = get_nth_level_expression(stream, n + 1);
        if (errno || left.type == EXPRESSION_ERROR)
          return left;

        token_t tok = next_token(stream); 
        if (tok.type == TOKEN_ARITHMETIC_OPERATOR &&
            (tok.operator== '+' || tok.operator == '-')) {
          expression_t right = get_nth_level_expression(stream, n);
          expression_t e;
          e.type = EXPRESSION_OPERATOR;
          e.operator = tok.operator;
          e.operands = malloc(2 * sizeof(*e.operands));
          if (!e.operands) {
            expression_destroy(left);
            expression_destroy(right);
            e.type = EXPRESSION_ERROR;
            return e;
          }
          e.operands[0] = left;
          e.operands[1] = right;

          return e;
        } else {
          push_back_token(tok);
          return left;
        }
      }
    case 2: 
      {
        expression_t left = get_nth_level_expression(stream, n + 1);
        if (errno || left.type == EXPRESSION_ERROR)
          return left;

        token_t tok = next_token(stream); 
        if (tok.type == TOKEN_ARITHMETIC_OPERATOR &&
            (tok.operator== '*' || tok.operator == '/')) {
          expression_t right = get_nth_level_expression(stream, n);
          expression_t e;
          e.type = EXPRESSION_OPERATOR;
          e.operator = tok.operator;
          e.operands = malloc(2 * sizeof(*e.operands));
          if (!e.operands) {
            expression_destroy(left);
            expression_destroy(right);
            e.type = EXPRESSION_ERROR;
            return e;
          }
          e.operands[0] = left;
          e.operands[1] = right;

          return e;
        } else {
          push_back_token(tok);
          return left;
        }
      }
    case 3: 
      {
        expression_t left = get_nth_level_expression(stream, n + 1);
        if (errno || left.type == EXPRESSION_ERROR)
          return left;

        token_t tok = next_token(stream); 
        if (tok.type == TOKEN_ARITHMETIC_OPERATOR && tok.operator == '^') {
          expression_t right = get_nth_level_expression(stream, n);
          expression_t e;
          e.type = EXPRESSION_OPERATOR;
          e.operator = tok.operator;
          e.operands = malloc(2 * sizeof(*e.operands));
          if (!e.operands) {
            expression_destroy(left);
            expression_destroy(right);
            e.type = EXPRESSION_ERROR;
            return e;
          }
          e.operands[0] = left;
          e.operands[1] = right;

          return e;
        } else {
          push_back_token(tok);
          return left;
        }
      }
    case 4: 
      {
        token_t tok = next_token(stream);
        expression_t e;
        switch (tok.type) {
          case TOKEN_NUMBER:
            e.type = EXPRESSION_NUMBER;
            e.d = tok.d;
            return e;
          case TOKEN_STRING:
            e.type = EXPRESSION_VARIABLE;
            e.s = tok.s;
            return e;
          case TOKEN_FUNCTION:
            e.type = EXPRESSION_FUNCTION;
            e.s = tok.s;
            expression_t operand = get_nth_level_expression(stream, n);
            if (operand.type == EXPRESSION_ERROR) {
              free(e.s);
              e.type = EXPRESSION_ERROR;
              return e;
            }

            e.operands = malloc(sizeof(*e.operands));
            if (!e.operands) {
              free(e.s);
              expression_destroy(operand);
              e.type = EXPRESSION_ERROR;
              return e;
            }

            e.operands[0] = operand;

            return e;
          case TOKEN_ARITHMETIC_OPERATOR:
            if (tok.operator == '(') {
              e = get_nth_level_expression(stream, 1);

              if (e.type == EXPRESSION_ERROR)
                return e;

              token_t paren = next_token(stream);
              if (paren.type != TOKEN_ARITHMETIC_OPERATOR || paren.operator != ')') {
                token_destroy(paren);
                expression_destroy(e);
                e.type = EXPRESSION_ERROR;
              }

              return e;
            } else if (tok.operator == '-')  {
              e.type = EXPRESSION_OPERATOR;
              e.operator = 'N';

              expression_t operand = get_nth_level_expression(stream, n);
              if (operand.type == EXPRESSION_ERROR) {
                e.type = EXPRESSION_ERROR;
                return e;
              }

              e.operands = malloc(sizeof(*e.operands));
              if (!e.operands) {
                expression_destroy(operand);
                e.type = EXPRESSION_ERROR;
                return e;
              }

              e.operands[0] = operand;

              return e;
            } else {
              e.type = EXPRESSION_ERROR;
              return e;
            }

            break;
          case TOKEN_UNKNOWN:
          case TOKEN_ERROR:
          case TOKEN_END:
            e.type = EXPRESSION_ERROR;
            return e;
        }
      }
  }

  expression_t e;
  e.type = EXPRESSION_ERROR;
  return e;
}

expression_t next_expression(FILE *const stream) {
  return get_nth_level_expression(stream, 0);
}
