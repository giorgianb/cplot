#include "../include/plot.h"
#include <stdio.h>
#include <string.h>

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

void plot(FILE *const stream, const plot_info_t plot, const point_t points[], const size_t npoints) {
  if (plot.y_number_width > plot.ncolumns / 5) {
    fputs("Error: specified number width to large.\n", stream);
    return;
  } else if (plot.nxticks > plot.ncolumns) {
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

  snprintf(xnformat, sizeof xnformat, "%%-%hu.%hulf", plot.x_number_width, plot.x_precision);
  snprintf(ynformat, sizeof ynformat, "%%%hu.%hulf", plot.y_number_width, plot.y_precision);
  snprintf(ysformat, sizeof ysformat, "%%%hus", plot.x_number_width);

  qsort(points, npoints, sizeof *points, point_cmp);

  set_color(stream, RED);
  for (unsigned short i = 0; i < plot.nrows - 1; ++i)  {
    if (i == 0) {
      set_color(stream, WHITE);
      fprintf(stream, ynformat, plot.y_max * 1.0);
      set_color(stream, GREEN);
      print_top_left_corner(stream);
    } else if (plot.nyticks != 1 && i != 0 && i % (plot.nrows / (plot.nyticks - 1)) == 0) {
      set_color(stream, WHITE);
      fprintf(stream, ynformat, ((plot.nrows - i) * 1.0 / plot.nrows) * (plot.y_max - plot.y_min) + plot.y_min);
      set_color(stream, GREEN);
      print_right_adjoiner(stream);
    } else {
      set_color(stream, WHITE);
      fprintf(stream, ysformat, " ");
      set_color(stream, GREEN);
      print_vertical_line(stream);
    }
    fputc('\n', stream);
  }
  set_color(stream, WHITE); 
  fprintf(stream, ynformat, plot.y_min * 1.0);
  set_color(stream, GREEN);
  print_bottom_left_corner(stream);

  unsigned short columns_left = plot.ncolumns - 1 - plot.y_number_width;
  for (unsigned short i = 0; i < columns_left - 1; ++i)
    if (plot.nxticks != 1 && (i + 1) % ((columns_left + 1) / (plot.nxticks - 1)) == 0)
      print_top_adjoiner(stream);
    else
      print_horizontal_line(stream);
  print_bottom_right_corner(stream);
  fputc('\n', stream);

  set_color(stream, WHITE);
  fprintf(stream, ysformat, " ");
  columns_left = plot.ncolumns - plot.y_number_width;
  for (unsigned short i = 0; i < columns_left - 1; ++i)
    if (plot.nxticks != 1 && i % (columns_left / (plot.nxticks - 1)) == 0) {
      fprintf(stream, xnformat, (i * 1.0 / columns_left) * (plot.x_max - plot.x_min) + plot.x_min);
      i += plot.y_number_width - 1;
    } else
      fputc(' ', stream);
  fprintf(stream, xnformat, plot.x_max);

  fputc('\n', stream);
  set_color(stream, NO_COLOR);
}
