#include <poormans.h>

#include "terrain.h"

static void display(BlockContext ctx) {
	poor_cell* cell = poor_at(ctx.sx, ctx.sy);
	cell->bg = POOR_YELLOW;
	cell->chr = ' ';
}

const BlockTable TB_DIRT = {.display = display};
