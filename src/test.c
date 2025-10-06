#define POOR_IMPLEMENTATION
#include "poormans.h"

int main(int argc, char* argv[]) {
	int start_x = 5, x = start_x, y = 5;
	for (poor_init(); poor_running(); poor_tick()) {
		poor_title("testing!!!");
		poor_at(x, y)->chr = '@';
		poor_at(x, y)->fg = POOR_RED;
		if (++x > POOR_REFRESH_HZ + start_x)
			poor_exit();
	}
	return EXIT_SUCCESS;
}
