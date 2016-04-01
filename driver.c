#include "library.h"

int main(int argc, char** argv)
{
	int i;

        clear_screen();
	init_graphics();

	char key;
	int x = (640-20)/2;
	int y = (480-20)/2;

	do
	{
	        key = getkey();
	        draw_pixel(300, 300, 31<<11);
	        draw_rect(10, 10, 100, 50, rgb_to_colort(31,63,0));
		fill_rect(100, 100, 100, 50, rgb_to_colort(0,43,0));
                draw_text(200, 200, "Press 'q' to quit", rgb_to_colort(31,0,0));
                draw_text(300, 300, "After 2000 ms, a new rect will draw", rgb_to_colort(31,0,0));
                draw_text(0, 350, "It may take a few sec to exit due to scheduling", rgb_to_colort(31,0,0));
		sleep_ms(2000);
		draw_rect(400,400,10,20, rgb_to_colort(0,0,31));
	} while(key != 'q');

	exit_graphics();
	clear_screen();

	return 0;

}
