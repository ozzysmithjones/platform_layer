#ifndef PLATFORM_H
#define PLATFORM_H
#include "fundamental.h"

#if defined(_WIN32)
#define PLATFORM_WINDOWS
#elif defined(__linux__)
#define PLATFORM_LINUX
#elif defined(__APPLE__)
#define PLATFORM_MACOS
#endif // WIN32

typedef enum {
    KEY_NONE,
    KEY_BACKSPACE = 8,
    KEY_TAB = 9,
    KEY_ENTER = 13,
    KEY_SHIFT = 16,
    KEY_CTRL = 17,
    KEY_ALT = 18,
    KEY_PAUSE = 19,
    KEY_CAPS_LOCK = 20,
    KEY_ESCAPE = 27,
    KEY_SPACE = 32,
    KEY_PAGE_UP = 33,
    KEY_PAGE_DOWN = 34,
    KEY_END = 35,
    KEY_HOME = 36,
    KEY_LEFT = 37,
    KEY_UP = 38,
    KEY_RIGHT = 39,
    KEY_DOWN = 40,
    KEY_SELECT = 41,
    KEY_PRINT = 42,
    KEY_EXEC = 43,
    KEY_PRINT_SCREEN = 44,
    KEY_INSERT = 45,
    KEY_DELETE = 46,
    KEY_HELP = 47,
    KEY_0 = 48,
    KEY_1 = 49,
    KEY_2 = 50,
    KEY_3 = 51,
    KEY_4 = 52,
    KEY_5 = 53,
    KEY_6 = 54,
    KEY_7 = 55,
    KEY_8 = 56,
    KEY_9 = 57,
    KEY_A = 65,
    KEY_B = 66,
    KEY_C = 67,
    KEY_D = 68,
    KEY_E = 69,
    KEY_F = 70,
    KEY_G = 71,
    KEY_H = 72,
    KEY_I = 73,
    KEY_J = 74,
    KEY_K = 75,
    KEY_L = 76,
    KEY_M = 77,
    KEY_N = 78,
    KEY_O = 79,
    KEY_P = 80,
    KEY_Q = 81,
    KEY_R = 82,
    KEY_S = 83,
    KEY_T = 84,
    KEY_U = 85,
    KEY_V = 86,
    KEY_W = 87,
    KEY_X = 88,
    KEY_Y = 89,
    KEY_Z = 90,
    KEY_LEFT_WINDOWS = 91,
    KEY_RIGHT_WINDOWS = 92,
    KEY_APPLICATION = 93,
    KEY_SLEEP = 95,
    KEY_NUMPAD_0 = 96,
    KEY_NUMPAD_1 = 97,
    KEY_NUMPAD_2 = 98,
    KEY_NUMPAD_3 = 99,
    KEY_NUMPAD_4 = 100,
    KEY_NUMPAD_5 = 101,
    KEY_NUMPAD_6 = 102,
    KEY_NUMPAD_7 = 103,
    KEY_NUMPAD_8 = 104,
    KEY_NUMPAD_9 = 105,
    KEY_MULTIPLY = 106,
    KEY_ADD = 107,
    KEY_SEPARATOR = 108,
    KEY_SUBTRACT = 109,
    KEY_DECIMAL = 110,
    KEY_DIVIDE = 111,
    KEY_F1 = 112,
    KEY_F2 = 113,
    KEY_F3 = 114,
    KEY_F4 = 115,
    KEY_F5 = 116,
    KEY_F6 = 117,
    KEY_F7 = 118,
    KEY_F8 = 119,
    KEY_F9 = 120,
    KEY_F10 = 121,
    KEY_F11 = 122,
    KEY_F12 = 123,
    KEY_F13 = 124,
    KEY_F14 = 125,
    KEY_F15 = 126,
    KEY_F16 = 127,
    KEY_F17 = 128,
    KEY_F18 = 129,
    KEY_F19 = 130,
    KEY_F20 = 131,
    KEY_F21 = 132,
    KEY_F22 = 133,
    KEY_F23 = 134,
    KEY_F24 = 135,
    KEY_NUM_LOCK = 144,
    KEY_SCROLL_LOCK = 145,
    KEY_LEFT_SHIFT = 160,
    KEY_RIGHT_SHIFT = 161,
    KEY_LEFT_CTRL = 162,
    KEY_RIGHT_CTRL = 163,
    KEY_LEFT_ALT = 164,
    KEY_RIGHT_ALT = 165,

    KEY_LEFT_MOUSE = 0x01,
    KEY_RIGHT_MOUSE = 0x02,
    KEY_MIDDLE_MOUSE = 0x04,
} keyboard_key;
#define KEY_COUNT 256

typedef struct {
    bool down : 1;
    bool up : 1;
} key_input;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t scroll_delta;
    bool is_cursor;
} mouse_input;

#define MAX_TYPED_CHARACTERS 128
DECLARE_CAPPED_ARRAY(wchar_t, typed_characters, MAX_TYPED_CHARACTERS)

typedef struct {
    typed_characters typed_characters;
    key_input keys[KEY_COUNT];
    mouse_input mouse;
    bool closed_window;
} user_input;

typedef enum {
    WINDOW_MODE_WINDOWED,
    WINDOW_MODE_FULLSCREEN,
    WINDOW_MODE_BORDERLESS,
} window_mode;

typedef struct {
    void* handle;
    user_input input;
} window;

result create_window(const char* title, uint32_t width, uint32_t height, window_mode mode, window* out_window);
void destroy_window(window* window);
void update_window_input(window* window);

#endif // PLATFORM_H
