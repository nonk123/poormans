#define POOR_IMPLEMENTATION
#include "poormans.h"

int main(int argc, char* argv[]) {
	int x = 5, y = 10;
	for (poor_init(); poor_running(); poor_tick()) {
		poor_title("testing!!!");
		poor_at(x, y / 2)->chr = '@';
		poor_at(x, y / 2)->fg = POOR_RED;

		if (poor_key_pressed(POOR_ESC) || (poor_key_down(POOR_LCTRL) && poor_key_pressed(POOR_C)))
			poor_exit();

		x += poor_key_down(POOR_KP_6) - poor_key_down(POOR_KP_4);
		y += poor_key_down(POOR_KP_2) - poor_key_down(POOR_KP_8);
	}
	return EXIT_SUCCESS;
}
