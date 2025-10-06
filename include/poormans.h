#pragma once

#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// TODO: support anything but Windows.
#include <windows.h>
#include <io.h>

#define POOR_MAX_WIDTH (200)
#define POOR_MAX_HEIGHT (100)
#define POOR_DISPLAY_AREA (POOR_MAX_WIDTH * POOR_MAX_HEIGHT)

#ifndef POOR_DEFAULT_TITLE
#define POOR_DEFAULT_TITLE "Poor Man's Graphics"
#endif

#ifndef POOR_REFRESH_HZ
#define POOR_REFRESH_HZ (60)
#endif

#define POOR_REFRESH_RATE (1000 / POOR_REFRESH_HZ)

typedef struct {
	uint8_t fg : 4, bg : 4, chr;
} poor_cell;

/// Return console buffer width in character increments.
int poor_width();

/// Return console buffer height in character increments.
int poor_height();

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

typedef struct {
	uint8_t buf[32];
} poor_kbd_state;

static HANDLE poor_input, poor_output;
static HWND poor_window;

static char poor_title_buf[128] = POOR_DEFAULT_TITLE;

static int poor_window_width = 0, poor_window_height = 0;
static poor_cell poor_front[POOR_DISPLAY_AREA] = {0}, poor_back[POOR_DISPLAY_AREA] = {0};

static poor_kbd_state poor_kbd_now = {0}, poor_kbd_just = {0};

static int poor_request_exit = 0;
static clock_t poor_frame_start;

static void poor_set_cursor_hidden(int value) {
	CONSOLE_CURSOR_INFO info = {0};
	info.dwSize = 100, info.bVisible = !value;
	SetConsoleCursorInfo(poor_output, &info);
}

static void poor_fetch_window_size() {
	CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
	GetConsoleScreenBufferInfo(poor_output, &csbi);
	int new_width = csbi.srWindow.Right - csbi.srWindow.Left + 1,
	    new_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	if (new_width > POOR_MAX_WIDTH)
		new_width = POOR_MAX_WIDTH;
	if (new_height > POOR_MAX_HEIGHT)
		new_height = POOR_MAX_HEIGHT;
	if (poor_window_width != new_width || poor_window_height != new_height) {
		poor_memset(poor_back, 0, sizeof(poor_back));
		poor_set_cursor_hidden(1);
	}
	poor_window_width = new_width, poor_window_height = new_height;
}

static poor_cell* poor_atEx(poor_cell* ptr, int x, int y) {
	static poor_cell deflt = {0};
	if (x < 0 || y < 0 || x >= poor_width() || y >= poor_height())
		return &deflt;
	return &ptr[y * poor_width() + x];
}

int poor_width() {
	return poor_window_width;
}

int poor_height() {
	return poor_window_height;
}

#endif

/// Request exit. Breaks `for`-loop boilerplate from the `poor_running` check.
void poor_exit()
#ifdef POOR_IMPLEMENTATION
{
	poor_request_exit = 1;
}
#else
	;
#endif

/// Get a pointer to the cell at specified coordinates. Points to a dummy cell if x/y are out of bounds.
poor_cell* poor_at(int x, int y)
#ifdef POOR_IMPLEMENTATION
{
	return poor_atEx(poor_front, x, y);
}
#else
	;
#endif

/// Set console window's title. Pass `NULL` to reset it to poormans' default.
void poor_title(const char* title)
#ifdef POOR_IMPLEMENTATION
{
	if (title == NULL)
		title = POOR_DEFAULT_TITLE;
	snprintf(poor_title_buf, sizeof(poor_title_buf), "%s", title);
}
#else
	;
#endif

#ifdef POOR_IMPLEMENTATION
static int poor_key_in(const poor_kbd_state* kbd, uint8_t scancode) {
	return !!(kbd->buf[scancode / 8] & (1 << (scancode % 8)));
}
#endif

/// Check if a key is held down. Pass one of the `POOR_*` keycode constants.
int poor_key_down(uint8_t scancode)
#ifdef POOR_IMPLEMENTATION
{
	return poor_key_in(&poor_kbd_now, scancode);
}
#else
	;
#endif

/// Check if a key was just pressed. Pass one of the `POOR_*` keycode constants.
int poor_key_pressed(uint8_t scancode)
#ifdef POOR_IMPLEMENTATION
{
	return poor_key_in(&poor_kbd_just, scancode);
}
#else
	;
#endif

/// Initialize poormans. Should be the initializer inside `for` boilerplate.
void poor_init()
#ifdef POOR_IMPLEMENTATION
{
	poor_input = GetStdHandle(STD_INPUT_HANDLE), poor_output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(poor_input, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	poor_window = GetConsoleWindow();
}
#else
	;
#endif

/// Return 0 if program requests exit. Should be the condition inside `for` boilerplate.
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
	if (poor_request_exit) {
		SetConsoleTextAttribute(poor_output, POOR_GRAY);
		poor_set_cursor_hidden(0);
	}
	return !poor_request_exit;
}
#else
	;
#endif

#ifdef POOR_IMPLEMENTATION

static void poor_handle_input() {
	DWORD count = 0;
	GetNumberOfConsoleInputEvents(poor_input, &count);
	if (!count)
		return;

	INPUT_RECORD records[10] = {0};
	ReadConsoleInput(poor_input, records, sizeof(records) / sizeof(*records), &count);
	poor_memset(&poor_kbd_just, 0, sizeof(poor_kbd_just));

	for (DWORD i = 0; i < count; i++) {
		if (records[i].EventType != KEY_EVENT)
			continue;
		KEY_EVENT_RECORD event = records[i].Event.KeyEvent;
		WORD kbd = event.wVirtualScanCode;
		if (kbd < 1 || kbd >= 256)
			continue;
		const uint8_t mask = 1 << (kbd % 8);
		if (event.bKeyDown) {
			poor_kbd_now.buf[kbd / 8] |= mask;
			poor_kbd_just.buf[kbd / 8] |= mask;
		} else
			poor_kbd_now.buf[kbd / 8] &= ~mask;
	}
}

static void poor_blit() {
	int console_x = -2, console_y = 0, console_fg = -1, console_bg = -1;
	for (int y = 0; y < poor_height(); y++)
		for (int x = 0; x < poor_width(); x++) {
			const poor_cell* front = poor_atEx(poor_front, x, y);
			poor_cell* back = poor_atEx(poor_back, x, y);
			if (!poor_memcmp(front, back, sizeof(poor_cell)))
				continue;

			if (console_y != y || console_x != x - 1)
				SetConsoleCursorPosition(poor_output, (COORD){.X = x, .Y = y});
			console_x = x, console_y = y;

			if (front->fg != console_fg || front->bg != console_bg)
				SetConsoleTextAttribute(poor_output, (front->bg << 4) | front->fg);
			console_fg = front->fg, console_bg = front->bg;

			write(1, &front->chr, 1);
			*back = *front;
		}
}

static void poor_end_frame() {
	const clock_t frame_end = clock();
	const uint32_t deltaMs = ((((uint32_t)frame_end) - ((uint32_t)poor_frame_start)) * 1000) / CLOCKS_PER_SEC;
	if (deltaMs < POOR_REFRESH_RATE)
		Sleep(POOR_REFRESH_RATE - deltaMs);
}

#endif

/// Finalize poormans frame. Should be the increment inside `for` boilerplate.
void poor_tick()
#ifdef POOR_IMPLEMENTATION
{
	SetConsoleTitle(poor_title_buf);
	poor_handle_input();
	poor_blit();
	poor_end_frame();
}
#else
	;
#endif

enum {
	POOR_ESC = 1,
	POOR_1,
	POOR_2,
	POOR_3,
	POOR_4,
	POOR_5,
	POOR_6,
	POOR_7,
	POOR_8,
	POOR_9,
	POOR_0,
	POOR_HYPHEN,
	POOR_EQUALS,
	POOR_BACKSPACE,
	POOR_TAB,
	POOR_Q,
	POOR_W,
	POOR_E,
	POOR_R,
	POOR_T,
	POOR_Y,
	POOR_U,
	POOR_I,
	POOR_O,
	POOR_P,
	POOR_LEFT_BRACKET,
	POOR_RIGHT_BRACKET,
	POOR_ENTER,
	POOR_LCTRL,
	POOR_A,
	POOR_S,
	POOR_D,
	POOR_F,
	POOR_G,
	POOR_H,
	POOR_J,
	POOR_K,
	POOR_L,
	POOR_SEMICOLON,
	POOR_QUOTE,
	POOR_GRAVE, // aka tilde
	POOR_LSHIFT,
	POOR_BACKSLASH,
	POOR_Z,
	POOR_X,
	POOR_C,
	POOR_V,
	POOR_B,
	POOR_N,
	POOR_M,
	POOR_COMMA,
	POOR_FULL_STOP,
	POOR_SLASH,
	POOR_RSHIFT,
	POOR_PRT_SCR,
	POOR_LALT,
	POOR_SPACEBAR,
	POOR_CAPS_LOCK,
	POOR_F1,
	POOR_F2,
	POOR_F3,
	POOR_F4,
	POOR_F5,
	POOR_F6,
	POOR_F7,
	POOR_F8,
	POOR_F9,
	POOR_F10,
	POOR_NUMLOCK,
	POOR_SCROLL_LOCK,
	POOR_KP_7,
	POOR_KP_8,
	POOR_KP_9,
	POOR_KP_MINUS,
	POOR_KP_4,
	POOR_KP_5,
	POOR_KP_6,
	POOR_KP_PLUS,
	POOR_KP_1,
	POOR_KP_2,
	POOR_KP_3,
	POOR_KP_0,
	POOR_KP_DOT,
};
