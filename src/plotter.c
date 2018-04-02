#include "plotter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

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
  return (column * 1.0 / (columns_left - 1)) * (plot.x_max - plot.x_min) + plot.x_min;
}

static inline double get_upper_x(const plot_info_t plot, const unsigned short column) {
  return get_lower_x(plot, column + 1);
}

static inline double get_lower_y(const plot_info_t plot, const unsigned short row) {
  const unsigned short rows_left = plot.nrows - 1;  /* -1 for the x-axis */
  return (row * 1.0 / (rows_left - 1)) * (plot.y_max - plot.y_min) + plot.y_min;
}

static inline double get_upper_y(const plot_info_t plot, const unsigned short row) {
  return get_lower_y(plot, row + 1);
}

static inline bool y_should_draw_tick(const plot_info_t plot, const unsigned short row) {
  const unsigned short rows_left = plot.nrows - 1;
  const double rows_per_tick = (rows_left + 1.0) / (plot.nyticks - 1);
  
  return floor(row / rows_per_tick) < floor((row + 1) / rows_per_tick);
}

static inline bool x_should_draw_tick(const plot_info_t plot, const unsigned short column) {
  const unsigned short columns_left = plot.ncolumns - 1;
  const double columns_per_tick = (columns_left + 1.0) / (plot.nxticks - 1);

  return floor(column / columns_per_tick) < floor((column + 1) / columns_per_tick) 
    || (column == 0) || column == columns_left - 1;
}

static inline bool x_should_draw_point(
    const plot_info_t plot, 
    const unsigned short row, 
    const unsigned short column, 
    const point_t point) {

  const double mx = pow(10, plot.x_precision);
  const double my = pow(10, plot.y_precision);

  const unsigned short rows_left = plot.nrows - 1;
  const unsigned short columns_left = plot.ncolumns - 1;

  const double lower_y = get_lower_y(plot, row);
  const double upper_y = get_upper_y(plot, row);

  const double lower_x = get_lower_x(plot, column);
  const double upper_x = get_upper_x(plot, column);

  return floor(point.y * my) >= floor(lower_y * my) 
    && floor(point.x * mx) >= floor(lower_x * mx)
    && (floor(point.x * mx) < floor(upper_x * mx)
        || (floor(point.x * mx) == floor(upper_x * mx)
          && column == columns_left - 1))
    && (floor(point.y * my) < floor(upper_y * my)
        || (floor(point.y * my) == floor(upper_y * my)
          && row == rows_left - 1));
}

static size_t draw_column(
    FILE *const stream,
    const plot_info_t plot, 
    const point_t points[], 
    const size_t npoints,
    const unsigned short row, 
    size_t index) {
  const unsigned short columns_left = plot.ncolumns - 1;

  for (unsigned short j = 0; j < columns_left; ++j) {
    bool drew = false;
    for (size_t i = 0; i < npoints && !drew; ++i)
      if (x_should_draw_point(plot, row, j, points[i])) {
        set_color(stream, plot.mark_color);
        fputc(plot.mark_char, stream);
        drew = true;
      }

    if (!drew)
      fputc(' ', stream);
  }

  fputc('\n', stream);

  return index;
}

void plot(FILE *const stream, const plot_info_t p, point_t points[], const size_t npoints) {
  if (p.nxticks > p.ncolumns) {
    fputs("Error: too many x-ticks.\n", stream);
    return;
  } else if (p.nyticks > p.nrows) {
    fputs("Error: too many y-ticks.\n", stream);
    return;
  } else if (p.nxticks < 2) {
    fputs("Error: too few x-ticks.\n", stream);
    return;
  } else if (p.nyticks < 2) {
    fputs("error: too few y-ticks.\n", stream);
    return;
  }
  char xnformat[20], ynformat[20], ysformat[20];


  // TODO: figure out string lengths
  snprintf(xnformat, sizeof xnformat, "%%-%hu.%hulf", p.x_number_width, p.x_precision);
  snprintf(ynformat, sizeof ynformat, "%%%hu.%hulf", p.y_number_width, p.y_precision);
  snprintf(ysformat, sizeof ysformat, "%%%hus", p.y_number_width);


  const unsigned short rows_left = p.nrows - 1;
  const unsigned short columns_left = p.ncolumns - 1;

  set_color(stream, p.y_number_color);
  fprintf(stream, ynformat, p.y_max * 1.0);
  set_color(stream, p.axes_color);
  print_top_left_corner(stream);
  size_t index = draw_column(stream, p, points, npoints, rows_left - 1, 0);
  for (unsigned short i = 1; i < rows_left - 1; ++i)  {
    const double lower_y = get_lower_y(p, rows_left - 1 - i);
    if (y_should_draw_tick(p, p.nrows - i - 2)) { 
      set_color(stream, p.y_number_color);
      fprintf(stream, ynformat, lower_y);
      set_color(stream, p.axes_color);
      print_right_adjoiner(stream);
    } else {
      set_color(stream, NO_COLOR);
      fprintf(stream, ysformat, " ");
      set_color(stream, p.axes_color);
      print_vertical_line(stream);
    }
    index = draw_column(stream, p, points, npoints, p.nrows - i - 2, index);
  } 

  set_color(stream, p.y_number_color); 
  fprintf(stream, ynformat, p.y_min * 1.0);
  set_color(stream, p.axes_color);
  print_right_adjoiner(stream);
  index = draw_column(stream, p, points, npoints, 0, index);

  set_color(stream, NO_COLOR);
  fprintf(stream, ysformat, " ");
  set_color(stream, p.axes_color);
  print_bottom_left_corner(stream);

  set_color(stream, p.axes_color);
  print_top_adjoiner(stream);
  for (unsigned short i = 1; i < columns_left - 1; ++i)
    if (x_should_draw_tick(p, i))
      print_top_adjoiner(stream);
    else
      print_horizontal_line(stream);
  print_bottom_right_corner(stream);
  fputc('\n', stream);

  set_color(stream, NO_COLOR);
  fprintf(stream, ysformat, " ");
  fputc(' ', stream);
  for (unsigned short i = 0; i < columns_left - 1; ++i)
    if (x_should_draw_tick(p, i)) {
      set_color(stream, p.x_number_color);
      fprintf(stream, xnformat, get_lower_x(p, i));
      i += p.x_number_width - 1;
    } else {
      set_color(stream, NO_COLOR);
      fputc(' ', stream);
    }
  set_color(stream, p.x_number_color);
  fprintf(stream, xnformat, get_lower_x(p, columns_left - 1));

  fputc('\n', stream);
  set_color(stream, NO_COLOR);
}
