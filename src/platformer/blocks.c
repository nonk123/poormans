#include "terrain.h"

extern const BlockTable TB_DIRT;
const BlockTable TB_AIR = {0};

const BlockTable* Blocks[BLOCK_MAX] = {
	[BLOCK_AIR] = &TB_AIR,
	[BLOCK_DIRT] = &TB_DIRT,
};
