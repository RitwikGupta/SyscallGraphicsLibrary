typedef unsigned short color_t;

void init_graphics();
void exit_graphics();
void clear_screen();
char getkey();
void sleep_ms(long ms);
void draw_pixel(int x, int y, color_t color);
void draw_rect(int x1, int y1, int width, int height, color_t c);
void fill_rect(int x1, int y1, int width, int height, color_t c);
void draw_char(int x, int y, char text, color_t c);
void draw_text(int x, int y, const char *text, color_t c);
color_t rgb_to_colort(int r, int g, int b);
color_t *fbxy_get(int x, int y);
