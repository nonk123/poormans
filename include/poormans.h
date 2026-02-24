#pragma once

#ifndef POOR_NOSTD
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#endif

// TODO: support anything other than Windows.
#include <windows.h>
#include <signal.h>
#include <io.h>

#define POOR_MAX_WIDTH (400)
#define POOR_MAX_HEIGHT (140)
#define POOR_DISPLAY_AREA (POOR_MAX_WIDTH * POOR_MAX_HEIGHT)

#ifndef POOR_DEFAULT_TITLE
#define POOR_DEFAULT_TITLE "Poor Man's Graphics"
#endif

#ifndef POOR_DEFAULT_REFRESH_HZ
#define POOR_DEFAULT_REFRESH_HZ (60)
#endif

#ifndef POOR_BATCH_SIZE
#define POOR_BATCH_SIZE (8192)
#endif

#define WHILE_POOR for (poor_init(); poor_running(); poor_tick())

typedef uint8_t poor_char;

typedef struct {
	uint8_t fg : 4, bg : 4;
	poor_char chr;
} poor_cell;

/// Return console buffer width in character increments.
int poor_width();

/// Return console buffer height in character increments.
int poor_height();

typedef enum {
	POOR_BLACK,
	POOR_RED,
	POOR_GREEN,
	POOR_YELLOW,
	POOR_BLUE,
	POOR_MAGENTA,
	POOR_CYAN,
	POOR_WHITE,
	POOR_BRIGHT_BLACK,
	POOR_GRAY = POOR_BRIGHT_BLACK,
	POOR_BRIGHT_RED,
	POOR_BRIGHT_GREEN,
	POOR_BRIGHT_YELLOW,
	POOR_BRIGHT_BLUE,
	POOR_BRIGHT_MAGENTA,
	POOR_BRIGHT_CYAN,
	POOR_BRIGHT_WHITE,
} poor_color;

typedef enum {
	POOR_KEY_MIN,
	POOR_ESC,
	POOR_DIGIT_1,
	POOR_DIGIT_2,
	POOR_DIGIT_3,
	POOR_DIGIT_4,
	POOR_DIGIT_5,
	POOR_DIGIT_6,
	POOR_DIGIT_7,
	POOR_DIGIT_8,
	POOR_DIGIT_9,
	POOR_DIGIT_0,
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
	POOR_KEY_MAX,
} poor_key;

#ifdef POOR_IMPLEMENTATION

#ifndef poor_memset
#define poor_memset memset
#endif

#ifndef poor_memcpy
#define poor_memcpy memcpy
#endif

typedef uint8_t poor_kbd_state[32];
typedef poor_cell poor_display[POOR_DISPLAY_AREA];

static HANDLE poor_input = INVALID_HANDLE_VALUE, poor_output = INVALID_HANDLE_VALUE;
static HWND poor_window = INVALID_HANDLE_VALUE;

static char poor_title_buf[128] = POOR_DEFAULT_TITLE;

static int poor_window_width = 0, poor_window_height = 0;
static poor_display poor_front = {0}, poor_back = {0};
static poor_kbd_state poor_kbd_now = {0}, poor_kbd_just = {0};

typedef enum {
	POOR_RUNNING,
	POOR_GRACEFUL_EXIT,
	POOR_ERROR_EXIT,
} poor_status_t;

static poor_status_t poor_status = POOR_RUNNING;
static clock_t poor_frame_start = 0;
static double poor_raw_dt = 1.f / POOR_DEFAULT_REFRESH_HZ;

static void poor_write(const char* s) {
	if (poor_output != INVALID_HANDLE_VALUE)
		WriteFile(poor_output, s, strlen(s), NULL, NULL);
}

static void poor_fetch_window_size() {
	CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
	GetConsoleScreenBufferInfo(poor_output, &csbi);
	int width = csbi.srWindow.Right - csbi.srWindow.Left + 1, height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	if (width > POOR_MAX_WIDTH)
		width = POOR_MAX_WIDTH;
	if (height > POOR_MAX_HEIGHT)
		height = POOR_MAX_HEIGHT;
	if (poor_window_width != width || poor_window_height != height)
		poor_memset(poor_back, 0, sizeof(poor_back));
	poor_window_width = width, poor_window_height = height;
}

static poor_cell* poor_at_pro(poor_cell* ptr, int x, int y) {
	static poor_cell deflt = {0};
	if (x < 0 || y < 0 || x >= poor_width() || y >= poor_height())
		return &deflt;
	return &ptr[y * POOR_MAX_WIDTH + x];
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
	poor_status = POOR_GRACEFUL_EXIT;
}
#else
	;
#endif

/// Get a pointer to the cell at specified coordinates. Points to a dummy cell if x/y are out of bounds.
poor_cell* poor_at(int x, int y)
#ifdef POOR_IMPLEMENTATION
{
	return poor_at_pro(poor_front, x, y);
}
#else
	;
#endif

/// Shorthand for `poor_at` which sets a cell's foreground color.
void poor_fg(int x, int y, poor_color fg)
#ifdef POOR_IMPLEMENTATION
{
	poor_at(x, y)->fg = fg;
}
#else
	;
#endif

/// Shorthand for `poor_at` which sets a cell's background color.
void poor_bg(int x, int y, poor_color bg)
#ifdef POOR_IMPLEMENTATION
{
	poor_at(x, y)->bg = bg;
}
#else
	;
#endif

/// Shorthand for `poor_at` which sets a cell's character.
void poor_ch(int x, int y, poor_char chr)
#ifdef POOR_IMPLEMENTATION
{
	poor_at(x, y)->chr = chr;
}
#else
	;
#endif

/// Print a string at specified coordinates.
///
/// Doesn't wrap or do anything smart. If you need colors and/or formatting, loop over the cells manually.
void poor_printf(int x, int y, const char* format, ...)
#ifdef POOR_IMPLEMENTATION
{
	static char buf[64] = {0};

	va_list args = {0};
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);

	for (char* ptr = buf; *ptr; ptr++, x++) {
		poor_bg(x, y, POOR_BLACK);
		poor_fg(x, y, POOR_WHITE);
		poor_ch(x, y, *ptr);
	}
}
#else
	;
#endif

/// Set console window's title. Pass `NULL` to reset it to poormans' default.
void poor_title(const char* title)
#ifdef POOR_IMPLEMENTATION
{
	if (!title)
		title = POOR_DEFAULT_TITLE;
	snprintf(poor_title_buf, sizeof(poor_title_buf), "%s", title);
}
#else
	;
#endif

#ifdef POOR_IMPLEMENTATION
static bool poor_key_in(const poor_kbd_state kbd, poor_key scancode) {
	return !!(kbd[scancode / 8] & (1 << (scancode % 8)));
}
#endif

/// Check if a key is held down. Pass one of the `POOR_*` keycode constants.
bool poor_key_down(poor_key scancode)
#ifdef POOR_IMPLEMENTATION
{
	return poor_key_in(poor_kbd_now, scancode);
}
#else
	;
#endif

/// Check if a key was just pressed. Pass one of the `POOR_*` keycode constants.
bool poor_key_pressed(poor_key scancode)
#ifdef POOR_IMPLEMENTATION
{
	return poor_key_in(poor_kbd_just, scancode);
}
#else
	;
#endif

#ifdef POOR_IMPLEMENTATION

static void poor_handle_break(int signal) {
	(void)signal, poor_exit();
}

#define poor_damn_it()                                                                                                 \
	do {                                                                                                           \
		poor_status = POOR_ERROR_EXIT;                                                                         \
		return;                                                                                                \
	} while (0)

#endif

/// Initialize poormans. Should be the initializer inside `for` boilerplate.
void poor_init()
#ifdef POOR_IMPLEMENTATION
{
	poor_output = GetStdHandle(STD_OUTPUT_HANDLE), poor_input = GetStdHandle(STD_INPUT_HANDLE);
	poor_window = GetConsoleWindow();

	if (poor_output == INVALID_HANDLE_VALUE || poor_input == INVALID_HANDLE_VALUE
		|| poor_window == INVALID_HANDLE_VALUE)
		poor_damn_it();

	DWORD cur_mode = 0;
	GetConsoleMode(poor_output, &cur_mode);
	SetConsoleMode(poor_output, cur_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);

	GetConsoleMode(poor_input, &cur_mode);
	SetConsoleMode(poor_input, (cur_mode | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS) & ~ENABLE_QUICK_EDIT_MODE);

	poor_write("\x1b[?1049h");

	signal(SIGBREAK, poor_handle_break);
	poor_memset(poor_back, 0, sizeof(poor_back));
}
#else
	;
#endif

#ifdef POOR_IMPLEMENTATION

static void poor_cleanup() {
	// Unset `SIGBREAK` handler only if it's our own. It wouldn't be nice to overwrite someone else's handler.
	void (*current_handler)(int) = signal(SIGBREAK, SIG_DFL);
	if (current_handler != poor_handle_break)
		signal(SIGBREAK, current_handler);
	poor_write("\x1B[?1049l");
}

static void poor_handle_input() {
	DWORD count = 0, i = 0;
	GetNumberOfConsoleInputEvents(poor_input, &count);
	if (!count)
		return;

	INPUT_RECORD records[10] = {0};
	ReadConsoleInput(poor_input, records, sizeof(records) / sizeof(*records), &count);
	poor_memset(&poor_kbd_just, 0, sizeof(poor_kbd_just));

	for (; i < count; i++) {
		if (records[i].EventType != KEY_EVENT)
			continue;
		KEY_EVENT_RECORD event = records[i].Event.KeyEvent;
		WORD kbd = event.wVirtualScanCode;
		if (kbd <= POOR_KEY_MIN || kbd >= POOR_KEY_MAX)
			continue;
		const uint8_t mask = 1 << (kbd % 8);
		if (event.bKeyDown) {
			poor_kbd_now[kbd / 8] |= mask;
			poor_kbd_just[kbd / 8] |= mask;
		} else {
			poor_kbd_now[kbd / 8] &= ~mask;
		}
	}
}

static int poor_vsync_refresh_rate() {
	if (poor_window == INVALID_HANDLE_VALUE)
		goto fail;

	HMONITOR hmon = MonitorFromWindow(poor_window, MONITOR_DEFAULTTONEAREST);
	if (hmon == NULL)
		goto fail;

	MONITORINFOEX mi;
	mi.cbSize = sizeof(mi);
	if (!GetMonitorInfo(hmon, (MONITORINFO*)&mi))
		goto fail;

	DEVMODE dm = {0};
	dm.dmSize = sizeof(dm);
	if (!EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm))
		goto fail;

	if (dm.dmDisplayFrequency <= 1)
		goto fail;

	return (int)dm.dmDisplayFrequency;

fail:
	return POOR_DEFAULT_REFRESH_HZ;
}

#endif

/// Return false if program requests exit. Should be the condition inside `for` boilerplate.
bool poor_running()
#ifdef POOR_IMPLEMENTATION
{
	if (poor_status) {
		poor_cleanup();
		return false;
	}

	poor_frame_start = clock();
	poor_fetch_window_size();
	for (int i = 0; i < POOR_DISPLAY_AREA; i++) {
		poor_front[i].fg = POOR_GRAY;
		poor_front[i].bg = POOR_BLACK;
		poor_front[i].chr = ' ';
	}
	poor_handle_input();
	return true;
}
#else
	;
#endif

#ifdef POOR_IMPLEMENTATION

static void poor_blit() {
	static char batch[POOR_BATCH_SIZE + 1] = {0}, cell[128] = {0};
	size_t point = snprintf(batch, sizeof(batch), "\x1b[?25l");

	for (int y = 0; y < poor_height(); y++)
		for (int x = 0; x < poor_width(); x++) {
			const poor_cell* front = poor_at_pro(poor_front, x, y);
			poor_cell* back = poor_at_pro(poor_back, x, y);

			if (front->fg == back->fg && front->bg == back->bg && front->chr == back->chr)
				continue;

			size_t wrote = snprintf(cell, sizeof(cell), "\x1b[0;%d;%dm\x1b[%d;%dH%c",
				30 + front->fg + 52 * (front->fg >= 8), 40 + front->bg + 52 * (front->bg >= 8), y + 1,
				x + 1, front->chr);

			if (point + wrote > POOR_BATCH_SIZE)
				goto flush;

			poor_memcpy(batch + point, cell, wrote);
			point += wrote;

			poor_memcpy(back, front, sizeof(poor_cell));
		}

flush:
	batch[point] = 0;
	poor_write(batch);
	FlushFileBuffers(poor_output);
}

static double poor_elapsed = 0.0, poor_fps = 0.0;
static uint64_t poor_ticks = 0;

static void poor_end_frame() {
	const double refresh_rate = 1.0 / (double)poor_vsync_refresh_rate();
	poor_raw_dt = ((double)(clock() - poor_frame_start)) / (double)CLOCKS_PER_SEC;
	if (poor_raw_dt < refresh_rate) {
		Sleep((DWORD)(1000 * (refresh_rate - poor_raw_dt)));
		poor_raw_dt = refresh_rate;
	}

	poor_ticks++, poor_elapsed += poor_raw_dt;
	if (poor_elapsed >= 1.0)
		poor_fps = (double)poor_ticks / poor_elapsed;
	while (poor_elapsed >= 1.0)
		poor_ticks = 0, poor_elapsed -= 1.0;
}

#endif

/// Finalize poormans frame. Should be the increment inside `for` boilerplate.
void poor_tick()
#ifdef POOR_IMPLEMENTATION
{
#if 1
	poor_printf(0, poor_height() - 1, "% 3dHz % 3.2fFPS", poor_vsync_refresh_rate(), poor_fps);
#endif
	SetConsoleTitle(poor_title_buf);
	poor_blit();
	poor_end_frame();
}
#else
	;
#endif

/// Get the time it took to render the previous frame, in seconds.
double poor_dt()
#ifdef POOR_IMPLEMENTATION
{
	return poor_raw_dt;
}
#else
	;
#endif

/// Same as `poor_dt`, but cast to float for convenience.
float poor_dtf()
#ifdef POOR_IMPLEMENTATION
{
	return (float)poor_raw_dt;
}
#else
	;
#endif
