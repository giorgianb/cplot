#include "tokenizer.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static token_t tok;
static bool pushed_back = false;

static bool is_defined_function (const char *const s);

void
push_back_token (const token_t token)
{
  tok = token;
  pushed_back = true;
}

token_t
next_token (FILE * const stream)
{
  if (pushed_back)
    {
      pushed_back = false;
      return tok;
    }

  int c;

  while (isspace (c = getc (stream)))
    ;

  if (c == EOF || c == ';')
    {
      tok.type = TOKEN_END;
      return tok;
    }

  if (isalpha (c))
    {
      size_t size = 16;
      size_t pos = 0;
      tok.s = malloc (sizeof (*tok.s) * size);
      tok.s[pos++] = c;
      while (isalpha (c = getc (stream)))
	{
	  if (pos >= size)
	    {
	      size *= 2;
	      char *const new_buf = realloc (tok.s, size);
	      if (!new_buf)
		{
		  free (tok.s);
		  tok.type = TOKEN_ERROR;
		  return tok;	/* let user examine errno to determine that there was no more memory */
		}
	      tok.s = new_buf;
	    }

	  tok.s[pos++] = c;
	}

      if (pos >= size)
	{
	  size += sizeof (*tok.s);
	  char *const new_buf = realloc (tok.s, size);
	  if (!new_buf)
	    {
	      free (tok.s);
	      tok.type = TOKEN_ERROR;
	      return tok;	/* let user examine errno to determine that there was no more memory */
	    }
	  tok.s = new_buf;
	}
      tok.s[pos++] = '\0';

      if (c != EOF && c != ungetc (c, stream))
	{
	  tok.type = TOKEN_ERROR;
	  return tok;		/* errno can be examined to determine error */
	}

      if (is_defined_function (tok.s))
	tok.type = TOKEN_FUNCTION;
      else
	tok.type = TOKEN_STRING;

      return tok;
    }
  else if (isdigit (c) || c == '.')
    {
      if (c != ungetc (c, stream))
	{
	  tok.type = TOKEN_ERROR;
	  return tok;
	}

      if (fscanf (stream, "%lf", &tok.d) != 1)
	{
	  tok.type = TOKEN_ERROR;
	  return tok;
	}

      tok.type = TOKEN_NUMBER;

      return tok;
    }
  else
    switch (c)
      {
      case '+':
      case '-':
      case '*':
      case '/':
      case '(':
      case ')':
      case '^':
	tok.operator = c;
	tok.type = TOKEN_ARITHMETIC_OPERATOR;
	return tok;
      default:
	tok.operator = c;
	tok.type = TOKEN_UNKNOWN;
	return tok;
      }
}

void
token_destroy (const token_t token)
{
  switch (token.type)
    {
    case TOKEN_FUNCTION:
    case TOKEN_STRING:
      free (tok.s);
      break;
    default:
      break;
    }
}

static bool
is_defined_function (const char *const s)
{
  static const char *defined_functions[] = {
    "sin", "tan", "cos", "arcsin", "arctan", "arccos", "ln"
  };

  for (size_t i = 0;
       i < sizeof (defined_functions) / sizeof (defined_functions[0]); ++i)
    if (strcmp (s, defined_functions[i]) == 0)
      return true;

  return false;
}
