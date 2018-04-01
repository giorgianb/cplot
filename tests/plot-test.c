#include <stdio.h>
#include "../include/plot.h"

int main(void) {
  plot_info_t p;
  p.nrows = 6;
  p.ncolumns = 11;
  p.y_number_width = 8;
  p.y_precision = 3;
  p.nxticks = 6;
  p.nyticks = 6;
  p.y_max = 6;
  p.y_min = 0;
  p.x_max = 6;
  p.x_min = 0;
  p.x_number_width = 3;
  p.x_precision = 3;

  point_t points[6];

  for (size_t i = 0; i < sizeof(points)/sizeof(points[0]); ++i) {
    points[i].x = i;
    points[i].y = i;
  }





  plot(stdout, p, points, sizeof(points)/sizeof(points[0]));
  return 0;

}
