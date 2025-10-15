#pragma once

#include <stdint.h>

#define CHUNK_WIDTH (32)
#define WORLD_HEIGHT (256)
#define SEA_LEVEL (96)

typedef uint8_t BlockId;
enum {
	BLOCK_AIR = 0,
	BLOCK_DIRT,
	BLOCK_MAX,
};

typedef struct {
	void* userdata;
	int x, y, sx, sy;
} BlockContext;
typedef struct {
	void (*display)(BlockContext);
} BlockTable;
extern const BlockTable* Blocks[BLOCK_MAX];

#define USERDATA_MAX (8)
typedef struct {
	uint8_t userdata[USERDATA_MAX];
	BlockId id;
} Block;

typedef struct {
	Block blocks[WORLD_HEIGHT];
} Column;

typedef struct Chunk {
	Column cols[CHUNK_WIDTH];
	int x_offset;
	struct Chunk* next;
} Chunk;

Chunk* block2chunk(int);
int is_block_loaded(int);
void load_block(int);
Block* block_at(int, int);
