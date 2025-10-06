#define POOR_IMPLEMENTATION
#include "poormans.h"

#include <math.h>

typedef float real;
typedef const real realc;

realc deg2rad = 3.141592f / 180.f, rad2deg = 1.f / deg2rad;
#define rsin sinf
#define rcos cosf

typedef struct {
	real x, y, angle, radius;
} entity_t;
static entity_t player = {0};
static realc fov = 45.f * deg2rad;

static char* map[] = {
	"#########",
	"#       #",
	"#       #",
	"#       #",
	"#       #",
	"#       #",
	"#       #",
	"#       #",
	"#########",
};
static int mapWidth = 0;
static const int mapHeight = sizeof(map) / sizeof(*map);

const poor_cell tile_display[256] = {
	['#'] = {.chr = '#'},
};

static char* map_at(int x, int y) {
	static char dummy;
	if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
		dummy = ' ';
		return &dummy;
	}
	return &map[y][x];
}

static char* map_atR(real x, real y) {
	return map_at((int)x, (int)y);
}

static void cast_ray(int colx) {
	realc hw = 0.5f * poor_width(), offset = ((real)(colx - hw)) / hw;
	realc angle = player.angle + fov * offset, step = 0.02f;
	realc dirx = rcos(angle), diry = rsin(angle), znear = 0.1f, zfar = 20.f;

	char tile = ' ';
	real dist = znear;
	for (; dist < zfar; dist += step) {
		realc x = player.x + dirx * dist, y = player.y + diry * dist;
		const char next = *map_atR(x, y);
		if (next != ' ') {
			tile = next;
			break;
		}
	}
	if (tile == ' ')
		return;

	const int colh = (1.f - (dist - znear) / zfar) * (real)poor_height();
	for (int i = 0; i < colh; i++) {
		const int coly = (poor_height() - colh) / 2 + i;
		*poor_at(colx, coly) = tile_display[tile];
	}

	poor_at(colx, 10)->chr = '0' + (int)dist; // FIXME: debug info
	poor_at(colx, 11)->chr = tile;            // FIXME: debug info

	int kkk = colh;
	poor_at(colx, 15)->chr = '0' + (kkk % 10); // FIXME: debug info
	kkk /= 10;
	poor_at(colx, 14)->chr = '0' + (kkk % 10); // FIXME: debug info
	kkk /= 10;
	poor_at(colx, 13)->chr = '0' + (kkk % 10); // FIXME: debug info
}

int main(int argc, char* argv[]) {
	if (mapHeight && !mapWidth)
		mapWidth = strlen(map[0]);

	player.x = 0.5 * (real)mapWidth, player.y = 0.5 * (real)mapHeight;
	player.radius = 0.3, player.angle = 0.0;

	for (poor_init(); poor_running(); poor_tick()) {
		poor_title("Wolfenstein -1");
		if (poor_key_down(POOR_ESC))
			poor_exit();

		player.angle += 0.1f * (real)(poor_key_down(POOR_KP_6) - poor_key_down(POOR_KP_4));

		realc mv = poor_key_down(POOR_KP_8) - poor_key_down(POOR_KP_2), ang = player.angle;
		realc vel = mv * 3.f / POOR_REFRESH_HZ, mx = rcos(ang), my = rsin(ang);
		player.x += mx * vel, player.y += my * vel;

		for (int x = 0; x < poor_width(); x++)
			cast_ray(x);
	}

	return EXIT_SUCCESS;
}
