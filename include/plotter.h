#ifndef __PLOT_INC
#define __PLOT_INC
#include <stdio.h>

enum plot_color {
  BLACK, RED, GREEN, ORANGE, BLUE, PURPLE, CYAN, LIGHT_GRAY, DARK_GRAY, LIGHT_RED, 
  LIGHT_GREEN, YELLOW, LIGHT_BLUE, LIGHT_PURPLE, LIGHT_CYAN, WHITE, NO_COLOR
};

struct plot_info {
  unsigned short nrows, ncolumns;
  unsigned short x_number_width, y_number_width;
  unsigned short x_precision, y_precision;
  unsigned short nxticks, nyticks;
  double x_min, x_max;
  double y_min, y_max;

  enum plot_color mark_color;
  char mark_char;
  enum plot_color line_color;
  enum plot_color x_number_color;
  enum plot_color y_number_color;
};

typedef struct plot_info plot_info_t;

struct point {
  double x;
  double y;
};

typedef struct point point_t;

void plot(FILE *const stream, const plot_info_t plot, point_t points[], const size_t npoints);
#endif
