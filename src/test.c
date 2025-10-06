#define POORMANS_IMPLEMENTATION
#include "poormans.h"

int main(int argc, char* argv[]) {
	for (poormans_init(); poormans_running(); poormans_tick()) {}
	return EXIT_SUCCESS;
}
