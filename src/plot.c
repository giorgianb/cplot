#include "../include/plot.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

static int point_cmp(const void *const p1, const void *const p2) {
  const point_t *const a1 = (const point_t *) p1;
  const point_t *const a2 = (const point_t *) p2;

  if (a1->y > a2->y)
    return -1;
  else if (a1->y == a2->y) {
    if (a1->x < a1->x)
      return -1;
    else if (a1->x == a1->x)
      return 0;
    else
      return 1;
  } else
    return 1;
}

static void set_color(FILE *const stream, enum plot_color color) {
  char *color_code = NULL;
  switch (color) {
    case BLACK:
      color_code = "0;30";
      break;
    case RED:
      color_code = "0;31";
      break;
    case GREEN:
      color_code = "0;32";
      break;
    case ORANGE:
      color_code = "0;33";
      break;
    case BLUE:
      color_code = "0;34";
      break;
    case PURPLE:
      color_code = "0;35";
      break;
    case CYAN:
      color_code = "0;36";
      break;
    case LIGHT_GRAY:
      color_code = "0;37";
      break;
    case DARK_GRAY:
      color_code = "1;30";
      break;
    case LIGHT_RED:
      color_code = "1;31";
      break;
    case LIGHT_GREEN:
      color_code = "1;32";
      break;
    case YELLOW:
      color_code = "1;33";
      break;
    case LIGHT_BLUE:
      color_code = "1;34";
      break;
    case LIGHT_PURPLE:
      color_code = "1;35";
      break;
    case LIGHT_CYAN:
      color_code = "1;37";
      break;
    case WHITE:
      color_code = "1;37";
      break;
    case NO_COLOR:
      /* fall through */
    default:
      color_code = "0";
      break;
  }
  
  fprintf(stream, "\033[%sm", color_code);
}

static inline void print_top_left_corner(FILE *const stream) {
  fprintf(stream, "\033(0\x6c\033(B");
}

static inline void print_top_right_corner(FILE *const stream) {
  fprintf(stream, "\033(0\x6b\033(B");
}

static inline void print_bottom_left_corner(FILE *const stream) {
  fprintf(stream, "\033(0\x6d\033(B");
}

static inline void print_bottom_right_corner(FILE *const stream) {
  fprintf(stream, "\033(0\x6a\033(B");
}

static inline void print_double_adjoiner(FILE *const stream) {
  fprintf(stream, "\033(0\x6e\033(B");
}

static inline void print_left_adjoiner(FILE *const stream) {
  fprintf(stream, "\033(0\x75\033(B");
}

static inline void print_right_adjoiner(FILE *const stream) {
  fprintf(stream, "\033(0\x74\033(B");
}

static inline void print_bottom_adjoiner(FILE *const stream) {
  fprintf(stream, "\033(0\x77\033(B");
}

static inline void print_top_adjoiner(FILE *const stream) {
  fprintf(stream, "\033(0\x76\033(B");
}

static inline void print_horizontal_line(FILE *const stream) {
  fprintf(stream, "\033(0\x71\033(B");
}

static inline void print_vertical_line(FILE *const stream) {
  fprintf(stream, "\033(0\x78\033(B");
}

static inline double get_lower_x(const plot_info_t plot, const unsigned short column) {
  const unsigned short columns_left = plot.ncolumns - 1;
  return (column * 1.0 / columns_left) * (plot.x_max - plot.x_min) + plot.x_min;
}

static inline double get_upper_x(const plot_info_t plot, const unsigned short column) {
  const unsigned short columns_left = plot.ncolumns - 1;
  return ((column + 1) * 1.0 / columns_left) * (plot.x_max - plot.x_min) + plot.x_min;
}

static inline double get_lower_y(const plot_info_t plot, const unsigned short row) {
  const unsigned short rows_left = plot.nrows - 1;  /* -1 for the x-axis */
  return ((rows_left - row) * 1.0 / rows_left) * (plot.y_max - plot.y_min) + plot.y_min;
}

static inline double get_upper_y(const plot_info_t plot, const unsigned short row) {
  const unsigned short rows_left = plot.nrows - 1;  /* -1 for the x-axis */
  return ((rows_left - row + 1) * 1.0 / rows_left) * (plot.y_max - plot.y_min) + plot.y_min;
}

static inline bool should_draw_tick(const double n, const double max_n, const unsigned short nticks) {
  const double ticks_per_n = max_n / (nticks - 1);
  return floor(n /  ticks_per_n) > floor((n - 1) / ticks_per_n);
}

static inline bool y_should_skip_point(
    const plot_info_t plot, 
    const double row, 
    const point_t point) {

  const double my = pow(10, plot.y_precision);
  const unsigned short rows_left = plot.nrows - 1;  /* -1 for the x-axis */
  const double upper_y = ((rows_left - row + 1) * 1.0 / rows_left) * (plot.y_max - plot.y_min) + plot.y_min;

  return floor(point.y * my) >= floor(upper_y * my);
}


static inline bool x_should_skip_point(
    const plot_info_t plot, 
    const unsigned short row, 
    const unsigned short column, 
    const point_t point) {

  const double mx = pow(10, plot.x_precision);
  const double my = pow(10, plot.y_precision);

  const double lower_y = get_upper_y(plot, row);
  const double lower_x = get_lower_x(plot, column);
  
  return floor(point.y * my) >= floor(lower_y * my) && floor(mx * point.x) < floor(mx * lower_x);
}

static inline bool x_should_draw_point(
    const plot_info_t plot, 
    const unsigned short row, 
    const unsigned short column, 
    const point_t point) {

  const double mx = pow(10, plot.x_precision);
  const double my = pow(10, plot.y_precision);

  const unsigned short columns_left = plot.ncolumns - 1;

  const double lower_y = get_lower_y(plot, row);
  const double upper_y = get_upper_y(plot, row);

  const double lower_x = get_lower_x(plot, column);
  const double upper_x = get_upper_x(plot, column);

  /* if (index < npoints && points[index].y >= lower_y && points[index].x >= lower_x
     && (points[index].x < upper_x || (points[index].x == upper_x && j == columns_left - 1))
     && points[index].y < upper_y) { */

  bool ret =  floor(point.y * my) >= floor(lower_y * my) 
    && floor(point.x * mx) >= floor(lower_x * mx)
    && (floor(point.x * mx) < floor(upper_x * mx)
        || (floor(point.x * mx) == floor(upper_x * mx)
          && column == columns_left - 1))
    && floor(point.y * my) < floor(upper_y * my);

//  printf("(%lf %lf) (%lf %lf) (%g %g) draw: %d\n", lower_x, lower_y, upper_x, upper_y, point.x, point.y, ret);
  return ret;
 
}

static size_t draw_column(
    FILE *const stream,
    const plot_info_t plot, 
    const point_t points[], 
    const size_t npoints,
    const unsigned short row, 
    size_t index) {
  const unsigned short columns_left = plot.ncolumns - 1;
  while (index < npoints && y_should_skip_point(plot, row, points[index])) 
    ++index;

  for (unsigned short j = 0; j < columns_left && index < npoints; ++j) {
    while (index < npoints && x_should_skip_point(plot, row, j, points[index]))
      ++index;

    if (index < npoints && x_should_draw_point(plot, row, j, points[index])) {
      set_color(stream, WHITE);
      fputc('X', stream);
      ++index;
    } else
      fputc(' ', stream);
  }

  fputc('\n', stream);

  return index;
}

void plot(FILE *const stream, const plot_info_t plot, point_t points[], const size_t npoints) {
  if (plot.nxticks > plot.ncolumns) {
    fputs("Error: too many x-ticks.\n", stream);
    return;
  } else if (plot.nyticks > plot.nrows) {
    fputs("Error: too many y-ticks.\n", stream);
    return;
  } else if (plot.nxticks < 2) {
    fputs("Error: too few x-ticks.\n", stream);
    return;
  } else if (plot.nyticks < 2) {
    fputs("error: too few y-ticks.\n", stream);
    return;
  }
  char xnformat[20], ynformat[20], ysformat[20];

  /* hack to fix later */
  plot_info_t p = plot;
  --p.nrows;

  snprintf(xnformat, sizeof xnformat, "%%-%hu.%hulf", p.x_number_width, p.x_precision);
  snprintf(ynformat, sizeof ynformat, "%%%hu.%hulf", p.y_number_width, p.y_precision);
  snprintf(ysformat, sizeof ysformat, "%%%hus", p.x_number_width);

  qsort(points, npoints, sizeof *points, point_cmp);

  set_color(stream, WHITE);
  fprintf(stream, ynformat, p.y_max * 1.0);
  set_color(stream, GREEN);
  print_top_left_corner(stream);
  size_t index = draw_column(stream, p, points, npoints, 0, 0);
  const unsigned short rows_left = p.nrows - 1;
  const unsigned short columns_left = p.ncolumns - 1;
  for (unsigned short i = 1; i < rows_left; ++i)  {
    const double lower_y = get_upper_y(plot, i);
    if (should_draw_tick(i - rows_left, rows_left, p.nyticks)) {
      set_color(stream, WHITE);
      fprintf(stream, ynformat, lower_y);
      set_color(stream, GREEN);
      print_right_adjoiner(stream);
    } else {
      set_color(stream, WHITE);
      fprintf(stream, ysformat, " ");
      set_color(stream, GREEN);
      print_vertical_line(stream);
    }
    index = draw_column(stream, p, points, npoints, i, index);
  } 

  set_color(stream, WHITE); 
  fprintf(stream, ynformat, p.y_min * 1.0);
  set_color(stream, GREEN);
  print_right_adjoiner(stream);
  index = draw_column(stream, p, points, npoints, p.nrows - 1, index);


  fprintf(stream, ysformat, " ");
  set_color(stream, GREEN);
  print_bottom_left_corner(stream);

  set_color(stream, GREEN);
  print_top_adjoiner(stream);
  for (unsigned short i = 1; i < columns_left - 1; ++i)
    if (should_draw_tick(i, columns_left, p.nxticks))
      print_top_adjoiner(stream);
    else
      print_horizontal_line(stream);
  print_bottom_right_corner(stream);
  fputc('\n', stream);

  set_color(stream, WHITE);
  fprintf(stream, ysformat, " ");
  fputc(' ', stream);
  for (unsigned short i = 0; i < columns_left - 1; ++i)
    if (should_draw_tick(i, columns_left, p.nxticks)) {
      fprintf(stream, xnformat, (i * 1.0 / columns_left) * (p.x_max - p.x_min) + p.x_min);
      i += p.x_number_width - 1;
    } else
      fputc(' ', stream);
  fprintf(stream, xnformat, p.x_max);

  fputc('\n', stream);
  set_color(stream, NO_COLOR);
}
