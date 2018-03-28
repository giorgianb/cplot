#ifndef __PLOT_INC
#define __PLOT_INC
struct plot_info {
  unsigned nrows, ncolumns;
  double min_x, max_x;
  double min_y, max_y;
};

typedef struct plot_info plot_info_t;

struct point {
  double x;
  double y;
};

typedef struct point point_t;

void plot(const plot_info_t plot, const point_t points[]);
#endif
