#include <stdio.h>
#include "../include/plot.h"

int main(void) {
  plot_info_t p;
  p.nrows = 40;
  p.ncolumns = 48;
  p.y_number_width = 8;
  p.y_precision = 3;
  p.nxticks = 5;
  p.nyticks = 5;
  p.y_max = 100;
  p.y_min = -100;
  p.x_max = 100;
  p.x_min = -100;
  p.x_number_width = 8;
  p.x_precision = 3;


  plot(stdout, p, NULL, 0);
  return 0;

}
