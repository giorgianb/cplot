#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "plotter.h"
#include "parser.h"

#define NELEMS(arr) (sizeof(arr)/sizeof(arr[0]))

enum command_line_options
{
  file, expression, x_min, x_max, y_min, y_max, x_ticks, y_ticks,
  x_number_color, y_number_color, axes_color, mark_color, rows, columns,
  x_number_width, y_number_width, x_precision, y_precision, mark_char, help
};

enum plot_color process_color (const char *const color);
point_t *read_points (FILE * const in, size_t * npoints);
double evaluate_expression (const expression_t exp, const double x);
bool check_parser_errors (const expression_t exp);
bool check_variables (const expression_t exp);

double find_x_min (const point_t * const points, const size_t npoints);
double find_x_max (const point_t * const points, const size_t npoints);
double find_y_min (const point_t * const points, const size_t npoints);
double find_y_max (const point_t * const points, const size_t npoints);

int
main (int argc, char *argv[])
{
  static const struct option long_options[] = {
    {"file", required_argument, NULL, file},
    {"expression", required_argument, NULL, expression},
    {"x-min", required_argument, NULL, x_min},
    {"x-max", required_argument, NULL, x_max},
    {"y-min", required_argument, NULL, y_min},
    {"y-max", required_argument, NULL, y_max},
    {"x-ticks", required_argument, NULL, x_ticks},
    {"y-ticks", required_argument, NULL, y_ticks},
    {"x-number-color", required_argument, NULL, x_number_color},
    {"y-number-color", required_argument, NULL, y_number_color},
    {"axes-color", required_argument, NULL, axes_color},
    {"mark-color", required_argument, NULL, mark_color},
    {"rows", required_argument, NULL, rows},
    {"columns", required_argument, NULL, columns},
    {"x-number-width", required_argument, NULL, x_number_width},
    {"y-number-width", required_argument, NULL, y_number_width},
    {"x-precision", required_argument, NULL, x_precision},
    {"y-precision", required_argument, NULL, y_precision},
    {"mark-char", required_argument, NULL, mark_char},
    {"help", no_argument, NULL, help},
    {0, 0, 0, 0}
  };

  bool read_from_stdin = true;
  bool read_from_file = false;
  char *file_name = NULL;
  bool from_expression = false;
  char *source_expression = NULL;
  bool x_min_set = false, x_max_set = false;
  bool y_min_set = false, y_max_set = false;

  plot_info_t p;

  p.x_min = -10;
  p.x_max = 10;
  p.y_min = -10;
  p.y_max = 10;
  p.nxticks = 6;
  p.nyticks = 6;
  p.x_number_color = RED;
  p.y_number_color = BLUE;
  p.axes_color = GREEN;
  p.mark_color = WHITE;
  p.mark_char = '+';
  p.nrows = 22;
  p.ncolumns = 42;
  p.x_number_width = 8;
  p.y_number_width = 8;
  p.x_precision = 3;
  p.y_precision = 3;

  for (;;)
    {
      int option_index = 0;

      int c = getopt_long (argc, argv, "", long_options, &option_index);
      if (c == -1)
	break;

      switch (c)
	{
	case file:
	  read_from_stdin = false;
	  from_expression = false;
	  read_from_file = true;
	  file_name = optarg;
	  break;
	case expression:
	  read_from_stdin = false;
	  read_from_file = false;
	  from_expression = true;
	  source_expression = optarg;
	  break;
	case x_min:
	  sscanf (optarg, "%lf", &p.x_min);
	  x_min_set = true;
	  break;
	case x_max:
	  sscanf (optarg, "%lf", &p.x_max);
	  x_max_set = true;
	  break;
	case y_min:
	  sscanf (optarg, "%lf", &p.y_min);
	  y_min_set = true;
	  break;
	case y_max:
	  sscanf (optarg, "%lf", &p.y_max);
	  y_max_set = true;
	  break;
	case x_ticks:
	  sscanf (optarg, "%hu", &p.nxticks);
	  break;
	case y_ticks:
	  sscanf (optarg, "%hu", &p.nyticks);
	  break;
	case x_number_color:
	  fflush (stdout);
	  p.x_number_color = process_color (optarg);
	  break;
	case y_number_color:
	  p.y_number_color = process_color (optarg);
	  break;
	case axes_color:
	  p.axes_color = process_color (optarg);
	  break;
	case mark_color:
	  p.mark_color = process_color (optarg);
	  break;
	case rows:
	  sscanf (optarg, "%hu", &p.nrows);
	  break;
	case columns:
	  sscanf (optarg, "%hu", &p.ncolumns);
	  break;
	case x_number_width:
	  sscanf (optarg, "%hu", &p.x_number_width);
	  break;
	case y_number_width:
	  sscanf (optarg, "%hu", &p.y_number_width);
	  break;
	case x_precision:
	  sscanf (optarg, "%hu", &p.x_precision);
	  break;
	case y_precision:
	  sscanf (optarg, "%hu", &p.y_precision);
	  break;
	case mark_char:
	  p.mark_char = optarg[0];
	  break;
	case help:
	  {
	    static const char *const help_message =
	      "--file, --file=\t\t\t\tread and plot points from a file.\n"
	      "--expression, --expression=\t\tgenerate points from given expression.\n"
	      "--x-min, --x-min=\t\t\tspecify minimum x-value.\n"
	      "--x-max, --x-max=\t\t\tspecify maximum x-value.\n"
	      "--y-min, --y-min=\t\t\tspecify minimum y-value.\n"
	      "--y-max, --y-max=\t\t\tspecify maximum y-value\n"
	      "--x-ticks, --x-ticks=\t\t\tspecify number of x-axis ticks.\n"
	      "--y-ticks, --y-ticks=\t\t\tspecify number of y-axis ticks.\n"
	      "--x-number-color, --x-number-color=\tspecify color used to print tick labels on x-axis.\n"
	      "--y-number-color, --y-number-color=\tspecify color used to print tick labels on y-axis.\n"
	      "--axes-color, --axes-color=\t\tspecify color used to print axes.\n"
	      "--mark-color, --mark-color=\t\tspecify color used to print marks on plot.\n"
	      "--rows, --rows=\t\t\t\tspecify number of rows y-axis uses.\n"
	      "--columns, --columns=\t\t\tspecify number of columns x-axis uses.\n"
	      "--x-number-width, --x-number-width=\tspecify width of tick labels on x-axis.\n"
	      "--y-number-width, --y-number-width=\tspecify width of tick labels on y-axis.\n"
	      "--x-precision, --x-precision=\t\tspecify number of decimal points to use when printing x-axis tick labels.\n"
	      "--y-precision, --y-precision=\t\tspecify number of decimal points to use when printing y-axis tick labels.\n"
	      "--mark-char, --mark-char=\t\tspecify marker character to use on plot.\n"
	      "--help\t\t\t\t\tprint this message.\n\n\n"
	      "The following colors may be passed to arguments requiring colors:\n"
	      "black red green orange blue purple cyan ligh-gray dark-gray light-red light-green yellow light-blue light-purple "
	      "light-cyan white no-color\n\n"
	      "By default, if neither --file or --expression is specified, points are read from standard input.";



	    puts (help_message);
	    exit (EXIT_SUCCESS);
	    break;
	  }
	}
    }

  size_t npoints;
  point_t *points = NULL;
  if (read_from_stdin)
    {
      npoints = 0;
      points = read_points (stdin, &npoints);
      if (!points)
	{
	  perror ("");
	  exit (EXIT_FAILURE);
	}

    }
  else if (read_from_file)
    {
      FILE *const file = fopen (file_name, "r");
      if (!file)
	{
	  perror ("");
	  exit (EXIT_FAILURE);
	}

      npoints = 0;
      points = read_points (file, &npoints);
      if (!points)
	{
	  fclose (file);
	  perror ("");
	  exit (EXIT_FAILURE);
	}

      fclose (file);
    }
  else if (from_expression)
    {
      FILE *const file =
	fmemopen (source_expression, strlen (source_expression), "r");
      if (!file)
	{
	  perror ("");
	  exit (EXIT_FAILURE);
	}

      const expression_t exp = next_expression (file);
      if (!check_parser_errors (exp))
	{
	  fputs ("Could not parse expression", stderr);
	  fclose (file);
	  expression_destroy (exp);
	  exit (EXIT_FAILURE);
	}
      else if (!check_variables (exp))
	{
	  fputs ("Unknown variable in expression", stderr);
	  fclose (file);
	  expression_destroy (exp);
	  exit (EXIT_FAILURE);
	}

      npoints = 50 * p.ncolumns;
      points = malloc (npoints * sizeof (*points));
      if (!points)
	{
	  perror ("");
	  fclose (file);
	}

      x_min_set = true;
      x_max_set = true;
      for (size_t i = 0; i < npoints; ++i)
	{
	  points[i].x = i * (p.x_max - p.x_min) / npoints + p.x_min;
	  points[i].y = evaluate_expression (exp, points[i].x);
	}

      expression_destroy (exp);
      fclose (file);
    }

  if (!x_min_set)
    p.x_min = find_x_min (points, npoints);
  if (!x_max_set)
    p.x_max = find_x_max (points, npoints);
  if (!y_min_set)
    p.y_min = find_y_min (points, npoints);
  if (!y_max_set)
    p.y_max = find_y_max (points, npoints);

  plot (stdout, p, points, npoints);
  free (points);

  exit (EXIT_SUCCESS);
}

enum plot_color
process_color (const char *const color)
{
  const char *colors_text[] = {
    "black", "red", "green", "orange", "blue", "purple", "cyan", "light-gray",
      "dark-gray", "light-red",
    "light-green", "yellow", "light-blue", "light-purple", "light-cyan",
      "white", "no-color"
  };

  enum plot_color colors[] =
  {
    BLACK, RED, GREEN, ORANGE, BLUE, PURPLE, CYAN, LIGHT_GRAY, DARK_GRAY,
      LIGHT_RED,
    LIGHT_GREEN, YELLOW, LIGHT_BLUE, LIGHT_PURPLE, LIGHT_CYAN, WHITE, NO_COLOR
  };

  for (size_t i = 0; i < NELEMS (colors_text); ++i)
    if (strcmp (colors_text[i], color) == 0)
      return colors[i];

  return NO_COLOR;
}

point_t *
read_points (FILE * const in, size_t * npoints)
{
  size_t size = 8;
  size_t index = 0;
  point_t *points = malloc (size * sizeof (*points));
  if (!points)
    return NULL;

  point_t p;
  while (fscanf (in, "%lf %lf", &p.x, &p.y) == 2)
    {
      if (index == size)
	{
	  size *= 2;
	  point_t *const buf = realloc (points, size * sizeof (*points));
	  if (!buf)
	    {
	      *npoints = index;
	      return points;
	    }
	  points = buf;
	}
      points[index++] = p;
    }

  *npoints = index;
  return points;
}

bool
check_parser_errors (const expression_t expression)
{
  switch (expression.type)
    {
    case EXPRESSION_FUNCTION:
      return check_parser_errors (expression.operands[0]);
    case EXPRESSION_OPERATOR:
      {
	bool ret = check_parser_errors (expression.operands[0]);
	if (expression.operator != 'N')
	  {
	    ret &= check_parser_errors (expression.operands[1]);
	  }
	return ret;
      }
    case EXPRESSION_NUMBER:
      return true;
    case EXPRESSION_ERROR:
      return false;
    case EXPRESSION_VARIABLE:
      return true;
    }

  return false;
}

bool
check_variables (const expression_t expression)
{
  switch (expression.type)
    {
    case EXPRESSION_FUNCTION:
      return check_variables (expression.operands[0]);
    case EXPRESSION_OPERATOR:
      {
	bool ret = check_variables (expression.operands[0]);
	if (expression.operator != 'N')
	  {
	    ret &= check_variables (expression.operands[1]);
	  }
	return ret;
      }
    case EXPRESSION_NUMBER:
      return true;
    case EXPRESSION_ERROR:
      return false;
    case EXPRESSION_VARIABLE:
      return strcmp (expression.s, "x") == 0;
    }

  return false;
}

double
dummy (double d)
{
  d = 0;
  return d;
}

typedef double (*mfptr) (double);

mfptr
get_trig_function (const char *const name)
{
  static const char *function_names[] = {
    "sin", "tan", "cos", "arcsin", "arctan", "arccos", "ln"
  };
  static const mfptr trig_functions[] = {
    sin, tan, cos, asin, atan, acos, log
  };

  for (size_t i = 0; i < NELEMS (function_names); ++i)
    if (strcmp (name, function_names[i]) == 0)
      return trig_functions[i];

  return dummy;
}

double
evaluate_expression (const expression_t exp, const double x)
{
  switch (exp.type)
    {
    case EXPRESSION_FUNCTION:
      return get_trig_function (exp.
				s) (evaluate_expression (exp.operands[0], x));
      break;
    case EXPRESSION_OPERATOR:
      switch (exp.operator)
	{
	case '+':
	  return evaluate_expression (exp.operands[0],
				      x) +
	    evaluate_expression (exp.operands[1], x);
	case '-':
	  return evaluate_expression (exp.operands[0],
				      x) -
	    evaluate_expression (exp.operands[1], x);
	case '/':
	  return evaluate_expression (exp.operands[0],
				      x) /
	    evaluate_expression (exp.operands[1], x);
	case '*':
	  return evaluate_expression (exp.operands[0],
				      x) *
	    evaluate_expression (exp.operands[1], x);
	case '^':
	  return pow (evaluate_expression (exp.operands[0], x),
		      evaluate_expression (exp.operands[1], x));
	case 'N':
	  return -evaluate_expression (exp.operands[0], x);
	default:
	  return 0;
	}
    case EXPRESSION_NUMBER:
      return exp.d;
    case EXPRESSION_VARIABLE:
      return x;
    default:
      return 0;
    }
}

double
find_x_min (const point_t * const points, const size_t npoints)
{
  if (npoints > 0)
    {
      double min = points[0].x;
      for (size_t i = 1; i < npoints; ++i)
	min = (points[i].x < min) ? points[i].x : min;

      return min;
    }

  return -10;
}

double
find_x_max (const point_t * const points, const size_t npoints)
{
  if (npoints > 0)
    {
      double max = points[0].x;
      for (size_t i = 1; i < npoints; ++i)
	max = (points[i].x > max) ? points[i].x : max;

      return max;
    }

  return 10;
}

double
find_y_min (const point_t * const points, const size_t npoints)
{
  if (npoints > 0)
    {
      double min = points[0].y;
      for (size_t i = 1; i < npoints; ++i)
	min = (points[i].y < min) ? points[i].y : min;

      return min;
    }

  return -10;
}

double
find_y_max (const point_t * const points, const size_t npoints)
{
  if (npoints > 0)
    {
      double max = points[0].y;
      for (size_t i = 1; i < npoints; ++i)
	max = (points[i].y > max) ? points[i].y : max;

      return max;
    }

  return 10;
}
