#pragma once

#define CHUNK_WIDTH (32)
#define WORLD_HEIGHT (256)
#define SEA_LEVEL (96)

typedef uint8_t BlockId;
enum {
	BLOCK_MAX,
}

typedef struct {
	int x, y;
} BlockContext;
typedef struct {
	void (*display)(const Block*, BlockContext);
} BlockTable;
extern BlockTable Blocks[BLOCK_MAX];

typedef struct {
	uint8_t userdata[8];
	BlockId id;
} Block;

typedef struct {
	Block blocks[WORLD_HEIGHT];
} Column;

typedef struct {
	Column cols[CHUNK_WIDTH];
} Chunk;
