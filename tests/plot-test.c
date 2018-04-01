#include <stdio.h>
#include "../include/plot.h"

int main(void) {
  plot_info_t p;
  p.nrows = 26;
  p.ncolumns = 42;
  p.y_number_width = 8;
  p.y_precision = 3;
  p.nxticks = 6;
  p.nyticks = 6;
  p.y_max = 60;
  p.y_min = -60;
  p.x_max = 60;
  p.x_min = -60;
  p.x_number_width = 8;
  p.x_precision = 3;
  p.x_number_color = RED;
  p.y_number_color = BLUE;
  p.line_color = PURPLE;
  p.mark_color = ORANGE;

  point_t points[120];

  for (int i = 0; i < sizeof(points)/sizeof(points[0]); ++i) {
    points[i].x = i - 60;
    points[i].y = i - 60;
  }





  plot(stdout, p, points, sizeof(points)/sizeof(points[0]));
  return 0;

}
