#include "terrain.h"

BlockTable Blocks[BLOCK_MAX] = {
	[BLOCK_AIR] = {.display = NULL},
	[BLOCK_DIRT] = {.display = display_dirt},
};
