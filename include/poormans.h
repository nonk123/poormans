#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// TODO: support anything but Windows.
#include <windows.h>
#define poor_sleepMs Sleep
#include <io.h>

#define POOR_MAX_WIDTH (200)
#define POOR_MAX_HEIGHT (100)
#define POOR_DISPLAY_AREA (POOR_MAX_WIDTH * POOR_MAX_HEIGHT)

#ifndef POOR_REFRESH_HZ
#define POOR_REFRESH_HZ (60)
#endif

#define POOR_REFRESH_RATE (1000 / POOR_REFRESH_HZ)

typedef struct {
	uint8_t fg : 4, bg : 4, chr;
} poor_cell;

enum {
	POOR_BLACK,
	POOR_BLUE,
	POOR_GREEN,
	POOR_AQUA,
	POOR_RED,
	POOR_PURPLE,
	POOR_YELLOW,
	POOR_WHITE,
	POOR_GRAY,
	POOR_BRIGHT_BLUE,
	POOR_BRIGHT_GREEN,
	POOR_BRIGHT_AQUA,
	POOR_BRIGHT_RED,
	POOR_BRIGHT_PURPLE,
	POOR_BRIGHT_YELLOW,
	POOR_BRIGHT_WHITE,
};

#ifdef POOR_IMPLEMENTATION

#ifndef poor_memset
#define poor_memset memset
#endif

#ifndef poor_memcpy
#define poor_memcpy memcpy
#endif

#ifndef poor_memcmp
#define poor_memcmp memcmp
#endif

static HANDLE poor_input, poor_output;
static HWND poor_window;
static int poor_width = 0, poor_height = 0;
static char poor_title[128] = "Poor Man's Graphics";
static poor_cell poor_front[POOR_DISPLAY_AREA] = {0}, poor_back[POOR_DISPLAY_AREA] = {0};

static void poor_fetch_window_size() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(poor_output, &csbi);
	int new_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int new_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	if (new_width > POOR_MAX_WIDTH)
		new_width = POOR_MAX_WIDTH;
	if (new_height > POOR_MAX_HEIGHT)
		new_height = POOR_MAX_HEIGHT;
	if (poor_width != new_width || poor_height != new_height)
		poor_memset(poor_back, 0, sizeof(poor_back));
	poor_width = new_width, poor_height = new_height;
}

static poor_cell* poor_atEx(poor_cell* ptr, int x, int y) {
	static poor_cell deflt = {0};
	if (x < 0 || y < 0 || x >= poor_width || y >= poor_height)
		return &deflt;
	return &ptr[y * poor_width + x];
}

static int poor_request_exit = 0;
static clock_t poor_frame_start;

#endif

void poor_exit()
#ifdef POOR_IMPLEMENTATION
{
	poor_request_exit = 1;
}
#else
	;
#endif

poor_cell* poor_at(int x, int y)
#ifdef POOR_IMPLEMENTATION
{
	return poor_atEx(poor_front, x, y);
}
#else
	;
#endif

void poor_init()
#ifdef POOR_IMPLEMENTATION
{
	poor_input = GetStdHandle(STD_INPUT_HANDLE);
	poor_output = GetStdHandle(STD_OUTPUT_HANDLE);
	poor_window = GetConsoleWindow();
	SetConsoleMode(poor_input, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
}
#else
	;
#endif

int poor_running()
#ifdef POOR_IMPLEMENTATION
{
	poor_frame_start = clock();
	poor_fetch_window_size();
	for (size_t i = 0; i < POOR_DISPLAY_AREA; i++) {
		poor_front[i].fg = POOR_GRAY;
		poor_front[i].bg = POOR_BLACK;
		poor_front[i].chr = ' ';
	}
	return !poor_request_exit;
}
#else
	;
#endif

void poor_tick()
#ifdef POOR_IMPLEMENTATION
{
	SetConsoleTitle(poor_title);

	int console_x = -2, console_y = 0, console_fg = -1, console_bg = -1;
	for (int y = 0; y < poor_height; y++)
		for (int x = 0; x < poor_width; x++) {
			const poor_cell* front = poor_atEx(poor_front, x, y);
			poor_cell* back = poor_atEx(poor_back, x, y);
			if (!poor_memcmp(front, back, sizeof(poor_cell)))
				continue;

			if (console_y != y || console_x != x - 1) {
				COORD coord;
				coord.X = x, coord.Y = y;
				SetConsoleCursorPosition(poor_output, coord);
			}
			console_x = x, console_y = y;

			if (front->fg != console_fg || front->bg != console_bg)
				SetConsoleTextAttribute(poor_output, 16 * front->bg + front->fg);
			console_fg = front->fg, console_bg = front->bg;

			write(1, &front->chr, 1);
			poor_memcpy(back, front, sizeof(poor_cell));
		}

	const clock_t frame_end = clock();
	const uint32_t deltaMs = ((((uint32_t)frame_end) - ((uint32_t)poor_frame_start)) * 1000) / CLOCKS_PER_SEC;
	if (deltaMs < POOR_REFRESH_RATE)
		poor_sleepMs(POOR_REFRESH_RATE - deltaMs);
}
#else
	;
#endif
