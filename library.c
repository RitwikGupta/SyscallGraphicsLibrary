#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <linux/fb.h>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#include "iso_font.h"

#define MAX_COL 479
#define MAX_ROW 679

extern int errno;

typedef unsigned short color_t;

struct fb_var_screeninfo var_si;
struct fb_fix_screeninfo stat_si;

struct termios with_echo;
struct termios without_echo;

fd_set rfs;

void *f_buffer;

int fb;

color_t rgb_to_colort(int r, int g, int b) {
  return (color_t) (r << 11) | (g << 5) | b;
}

void init_graphics() {
  fb = open("/dev/fb0", O_RDWR);
  if (fb < 0) {
    return;
  }

  if (ioctl(fb, FBIOGET_VSCREENINFO, &var_si) < 0) {
    close(fb);
    return;
  }

  if (ioctl(fb, FBIOGET_FSCREENINFO, &stat_si) < 0) {
    close(fb);
    return;
  }

  int buffer_size = var_si.yres_virtual * stat_si.line_length;

  f_buffer = mmap(NULL, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, 
  fb, 0);

  if (f_buffer == NULL) {
    return;
  }

  if (ioctl(0, TCGETS, &with_echo) < 0) {
    return;
  }
  without_echo = with_echo;
  without_echo.c_lflag &= ~(ICANON | ECHO);
  if (ioctl(0, TCSETS, &without_echo) < 0) {
    return;
  }  // disable echo
}

void exit_graphics() { 
  if (ioctl(0, TCSETS, &with_echo) < 0) {
    return;
  } // re-enable echo

  munmap(f_buffer, 0);
  close(fb);
}

void clear_screen() {
  write(0, "\033[2J", 4);
}

char getkey() {
  char ret = '\0';

  FD_ZERO(&rfs);
  FD_SET(0, &rfs);

  struct timeval tv = {0};

  if(select(1, &rfs, NULL, NULL, &tv)) {
     read(0, &ret, sizeof(char));
  }

  return ret;
}

void sleep_ms(long ms) {
  struct timespec sv = {0};
  struct timespec rm = {0};

  time_t sec = (int) ms / 1000;
  ms = (ms % 1000) * 1000000L;

  sv.tv_sec = sec;
  sv.tv_nsec = ms * 1000000L;

  while (nanosleep(&sv, &rm) == -1) {
    continue;
  }
}

color_t *fbxy_get(int x, int y) {
  return (color_t *)f_buffer + x + (y/2) * stat_si.line_length;
}

void draw_pixel(int x, int y, color_t c) {
  *(fbxy_get(x, y)) = c;
}

void draw_rect(int x1, int y1, int width, int height, color_t c) {
  int i = 0;

  for (i = x1; i <= x1 + width; i++) {
    draw_pixel(i, y1, c);
    draw_pixel(i, y1 + height, c);
  }

  for (i = y1; i <= y1 + height; i++) {
    draw_pixel(x1, i, c);
    draw_pixel(x1 + width, i, c);
  }
}

void fill_rect(int x1, int y1, int height, int width, color_t c) {
  int i = 0;
  int j = 0;

  for (i = x1; i <= x1 + width; i++) {
    for (j = y1; j <= y1 + height; j++) {
      draw_pixel(i, j , c);
    }
  }
}

void draw_char(int x, int y, char text, color_t c) {
  int i = 0;
  int j = 0;
  int tx = 0;
  int ty = 0;

  for(i = 0; i < 15; i++) {
    int s = iso_font[text * 16 + i];

    tx = x;
    ty = y + i;
    for(j = 0; j < 8; j++) {
      tx++;
      if (s & 1<<j) {
        draw_pixel(tx, ty, c);
      }
    }
  }
}

void draw_text(int x, int y, const char *text, color_t c) {
  int i = 0;

  while (text[i] != '\0') {
    draw_char(x, y, text[i], c);
    x += 8;
    i++;
  }
}
