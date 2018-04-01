#include <stdio.h>
#include "../include/plot.h"

int main(void) {
  plot_info_t p;
  p.nrows = 22;
  p.ncolumns = 61;
  p.y_number_width = 8;
  p.y_precision = 3;
  p.nxticks = 11;
  p.nyticks = 5;
  p.y_max = 60;
  p.y_min = 0;
  p.x_max = 60;
  p.x_min = 0;
  p.x_number_width = 8;
  p.x_precision = 3;

  point_t points[61];

  for (size_t i = 0; i < sizeof(points)/sizeof(points[0]); ++i) {
    points[i].x = i;
    points[i].y = i;
  }





  plot(stdout, p, points, sizeof(points)/sizeof(points[0]));
  return 0;

}
