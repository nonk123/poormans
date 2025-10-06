#define POOR_IMPLEMENTATION
#include "poormans.h"

int main(int argc, char* argv[]) {
	int x = 5, y = 5;
	for (poor_init(); poor_running(); poor_tick()) {
		poor_title("testing!!!");
		poor_at(x, y)->chr = '@';
		poor_at(x, y)->fg = POOR_RED;
		if (poor_key_pressed(POOR_ESC) || (poor_key_down(POOR_LCTRL) && poor_key_pressed(POOR_C)))
			poor_exit();
		x++;
	}
	return EXIT_SUCCESS;
}
