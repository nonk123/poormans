#include <poormans.h>

#include "terrain.h"

void tick() {
	static int center_x = 0;
	center_x += poor_key_down(POOR_KP_6) - poor_key_down(POOR_KP_4);

	const int start_x = center_x - poor_width() / 2;
	for (int dx = 0; dx < poor_width(); dx++)
		load_block(start_x + dx);

	const int center_y = SEA_LEVEL;
	const int start_y = center_y - poor_height() / 2;
	for (int dy = 0; dy < poor_height(); dy++) {
		for (int dx = 0; dx < poor_width(); dx++) {
			const int x = start_x + dx, y = start_y + dy;
			if (y < 0 || y >= WORLD_HEIGHT)
				goto next_y;
			Block* block = block_at(x, y);
			if (block == NULL)
				continue;
			const BlockTable* tb = Blocks[block->id];
			if (tb->display == NULL) {
				poor_at(dx, dy)->bg = POOR_AQUA;
				continue;
			}
			BlockContext ctx = {0};
			ctx.x = x, ctx.y = y;
			ctx.sx = dx, ctx.sy = dy;
			tb->display(ctx);
		}
	next_y:
		continue;
	}
}
