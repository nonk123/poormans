#define POOR_IMPLEMENTATION
#include <poormans.h>

int main(int argc, char* argv[]) {
	(void)argc, (void)argv;

	int x = 5, y = 10, counter = 1;
	WHILE_POOR {
		x += poor_key_down(POOR_KP_6) - poor_key_down(POOR_KP_4);
		y += poor_key_down(POOR_KP_2) - poor_key_down(POOR_KP_8);

		poor_ch(x, y / 2, '@');
		poor_fg(x, y / 2, POOR_BRIGHT_RED);
		poor_title("testing!!!");

		if (poor_key_pressed(POOR_SPACEBAR))
			counter++;
		poor_printf(0, 0, "SPC %d", counter);

		if (poor_key_pressed(POOR_ESC) || (poor_key_down(POOR_LCTRL) && poor_key_pressed(POOR_C)))
			poor_exit();
	}

	return EXIT_SUCCESS;
}
