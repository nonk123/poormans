#define S_FIXED_IMPLEMENTATION
#include "S_fixed.h"

#define POOR_IMPLEMENTATION
#include "poormans.h"

int main(int argc, char* argv[]) {
	for (poor_init(); poor_running(); poor_tick()) {
		poor_title("platforming!!!");
		if (poor_key_pressed(POOR_ESC) || (poor_key_down(POOR_LCTRL) && poor_key_pressed(POOR_C)))
			poor_exit();
	}
	return EXIT_SUCCESS;
}
