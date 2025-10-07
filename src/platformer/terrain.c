#include <stdlib.h>
#include <string.h>

#include <open-simplex-noise.h>
#include <poormans.h>

#include "terrain.h"

static Chunk* root = NULL;

Chunk* block2chunk(int x) {
	for (Chunk* c = root; c != NULL; c = c->next)
		if (x >= c->start_x && x < c->start_x + CHUNK_WIDTH)
			return c;
	return NULL;
}

int is_block_loaded(int x) {
	return block2chunk(x) != NULL;
}

#define NOISE_SPREAD 64.f
static float sample(int x) { // 0 to 1
	static struct osn_context* ctx = NULL;
	if (ctx == NULL)
		open_simplex_noise(77374, &ctx); // TODO: randomize seed
	if (ctx == NULL)
		poor_exit(); // TODO: handle error...
	return 1.f + 0.5f * open_simplex_noise2(ctx, ((float)x) / NOISE_SPREAD, 0.f);
}

#define BLOCK(y) (&chunk->cols[x - start_x].blocks[(y)])
static Chunk* generate_chunk(int start_x) {
	Chunk* chunk = malloc(sizeof(*chunk));
	memset(chunk, 0, sizeof(*chunk));
	chunk->start_x = start_x;

	for (int x = start_x; x < start_x + CHUNK_WIDTH; x++) {
		const int height = 1.f + sample(x) * (WORLD_HEIGHT - SEA_LEVEL) * 0.7f;
		for (int y = 0; y < SEA_LEVEL + height; y++)
			BLOCK(y)->id = BLOCK_DIRT;
	}

	return chunk;
}
#undef BLOCK

void load_block(int x) {
	if (is_block_loaded(x))
		return;
	const int start_x = (x / CHUNK_WIDTH) * CHUNK_WIDTH;
	Chunk* new = generate_chunk(start_x);
	new->next = root;
	root = new;
}

Block* block_at(int x, int y) {
	if (y < 0 || y >= WORLD_HEIGHT)
		return NULL;
	Chunk* ch = block2chunk(x);
	if (ch == NULL)
		return NULL;
	x -= ch->start_x;
	return &ch->cols[x].blocks[y];
}
