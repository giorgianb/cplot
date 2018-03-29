#ifndef __PLOT_INC
#define __PLOT_INC
#include <stdio.h>

struct plot_info {
  unsigned short nrows, ncolumns;
  unsigned short x_number_width, y_number_width;
  unsigned short x_precision, y_precision;
  unsigned nxticks, nyticks;
  double x_min, x_max;
  double y_min, y_max;
};

typedef struct plot_info plot_info_t;

struct point {
  double x;
  double y;
};

typedef struct point point_t;

void plot(FILE *const stream, const plot_info_t plot, point_t points[], const size_t npoints);

enum plot_color {
  BLACK, RED, GREEN, ORANGE, BLUE, PURPLE, CYAN, LIGHT_GRAY, DARK_GRAY, LIGHT_RED, 
  LIGHT_GREEN, YELLOW, LIGHT_BLUE, LIGHT_PURPLE, LIGHT_CYAN, WHITE, NO_COLOR
};
#endif
