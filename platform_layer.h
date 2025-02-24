#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdint.h>
#ifndef CPLUSPLUS
#include <stdbool.h>
#endif
#include <assert.h>

// Supported platforms:
#ifdef _WIN32
// Windows:
#define WIN32_LEAN_AND_MEAN
#define NO_MINMAX
#include <windows.h>
#include <windowsx.h>
#endif

typedef struct file_mapping file_mapping;
typedef struct dynamic_library dynamic_library;
typedef struct arena_allocator arena_allocator;
typedef struct window window;
typedef struct mutex mutex;
typedef struct thread thread;
typedef struct user_input user_input;

// can redefine this macro to change the logging behavior
#ifndef LOG_ERROR(message)
#ifndef NDEBUG
#include <stdio.h>
#define LOG_ERROR(message) fprintf(stderr, "error: %s\n", message)
#else
#define LOG_ERROR(message)
#endif
#endif

typedef enum result {
    FAILURE,
    SUCCESS,
} result;

typedef enum keyboard_key {
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
    KEY_EXEC  = 43,
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
} keyboard_key;

typedef enum window_mode {
    WINDOW_MODE_WINDOWED,
    WINDOW_MODE_BORDERLESS,
    WINDOW_MODE_FULLSCREEN,
} window_mode;

typedef struct key_input {
    bool up: 1;
    bool down: 1;
} key_input;

typedef struct mouse_input {
    int x;
    int y;
    int wheel;
    bool left_button_down;
    bool left_button_up;
    bool right_button_down;
    bool right_button_up;
    bool middle_button_down;
    bool middle_button_up;
} mouse_input;

typedef struct user_input {
    key_input keys[256];
    mouse_input mouse;
    bool closed_window;
} user_input;

typedef struct string {
    size_t length;
    const char* data;
} string;

// File mapping for platform independent code
result map_file(file_mapping* mapping, const char* path);
void unmap_file(file_mapping* mapping);

// Dynamic library loading for platform independent code
result load_dynamic_library(dynamic_library* library, const char* path);
void* dynamic_library_get_function(dynamic_library* library, const char* name);
void unload_dynamic_library(dynamic_library* library);

// Memory arena for platform independent code.
// if size is 0, the default page size is reserved.
result create_arena_allocator(arena_allocator* arena, size_t size);
void arena_align(arena_allocator* arena, size_t alignment);
void* arena_allocate_unaligned(arena_allocator* arena, void* source, size_t size);
void* arena_allocate(arena_allocator* arena, void* source, size_t size, size_t alignment);
void destroy_arena_allocator(arena_allocator* arena);

// Window creation for platform independent code
result create_window(window* window, const char* title, int width, int height, window_mode mode);
void update_window(window* window);
void set_window_mode(window* window, window_mode mode);
void destroy_window(window* window);

// Multithreading for platform independent code
void create_mutex(mutex* mutex);
void lock_mutex(mutex* mutex);
void unlock_mutex(mutex* mutex);
void destroy_mutex(mutex* mutex);
result create_thread(thread* thread, unsigned long (*function)(void*), void* data);
void destroy_thread(thread* thread);

// Windows platform code
#ifdef _WIN32

struct file_mapping {
    struct {
        HANDLE file;
        HANDLE mapping;
        LPVOID base_pointer;
        LARGE_INTEGER file_size;
    } platform_dependent;
    string text;
};

struct dynamic_library {
    struct {
        HMODULE handle;
    } platform_dependent;
};

struct arena_allocator {
    uintptr_t memory;
    uintptr_t memory_end;
    uintptr_t bump_pointer;
    uintptr_t next_page;
    size_t page_size;
};

struct window {
    struct {
        HWND handle;
        HINSTANCE instance;
    } platform_dependent;
    user_input input;
    window_mode mode;
};

struct mutex {
    struct {
        CRITICAL_SECTION critical_section;
    } platform_dependent;
};

struct thread {
    struct {
        HANDLE handle;
    } platform_dependent;
};

#endif // _WIN32
#endif // PLATFORM_H
//#define PLATFORM_IMPLEMENTATION
#ifdef PLATFORM_IMPLEMENTATION

// File mapping for platform independent code
result map_file(file_mapping* mapping, const char* path) {
    mapping->platform_dependent.file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (mapping->platform_dependent.file == INVALID_HANDLE_VALUE) {
        LOG_ERROR("Failed to open file");
        return FAILURE;
    }
    GetFileSizeEx(mapping->platform_dependent.file, &mapping->platform_dependent.file_size);
    mapping->platform_dependent.mapping = CreateFileMappingA(mapping->platform_dependent.file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (mapping->platform_dependent.mapping == NULL) {
        LOG_ERROR("Failed to create file mapping");
        return FAILURE;
    }
    mapping->platform_dependent.base_pointer = MapViewOfFile(mapping->platform_dependent.mapping, FILE_MAP_READ, 0, 0, 0);
    if (mapping->platform_dependent.base_pointer == NULL) {
        LOG_ERROR("Failed to map view of file");
        return FAILURE;
    }
    mapping->text.length = (size_t)mapping->platform_dependent.file_size.QuadPart;
    mapping->text.data = (const char*)mapping->platform_dependent.base_pointer;
    return SUCCESS;
}

void unmap_file(file_mapping* mapping) {
    UnmapViewOfFile(mapping->platform_dependent.base_pointer);
    CloseHandle(mapping->platform_dependent.mapping);
    CloseHandle(mapping->platform_dependent.file);
}

// Dynamic library loading for platform independent code
result load_dynamic_library(dynamic_library* library, const char* path) {
    library->platform_dependent.handle = LoadLibraryA(path);
    if (library->platform_dependent.handle == NULL) {
        LOG_ERROR("Failed to load dynamic library");
        return FAILURE;
    }
    return SUCCESS;
}

void* dynamic_library_get_function(dynamic_library* library, const char* name) {
    return GetProcAddress(library->platform_dependent.handle, name);
}

void unload_dynamic_library(dynamic_library* library) {
    FreeLibrary(library->platform_dependent.handle);
}

// Memory arena for platform independent code.
// if size is 0, the default page size is reserved.
result create_arena_allocator(arena_allocator* arena, size_t size) {
    result error = SUCCESS;
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    arena->page_size = system_info.dwPageSize;
    if (size == 0) {
        size = arena->page_size;
    }
    arena->memory = (uintptr_t)VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
    if (arena->memory == 0) {
        LOG_ERROR("Failed to reserve memory");
        return FAILURE;
    }
    arena->memory_end = arena->memory + size;
    arena->bump_pointer = arena->memory;
    arena->next_page = arena->memory;
    return SUCCESS;
}

void arena_align(arena_allocator* arena, size_t alignment) {
    size_t mask = alignment - 1;
    arena->bump_pointer = (uintptr_t)(((arena->bump_pointer + mask) & ~mask));
}

static int page_fault_handler(arena_allocator* arena, DWORD error_code) {
	if (error_code != EXCEPTION_ACCESS_VIOLATION) {
		return EXCEPTION_EXECUTE_HANDLER;
	}
	if (arena->next_page >= arena->memory_end) {
		return EXCEPTION_EXECUTE_HANDLER;
	}
	void* page = VirtualAlloc((void*)arena->next_page, arena->page_size, MEM_COMMIT, PAGE_READWRITE);
	if (page == NULL) {
		return EXCEPTION_EXECUTE_HANDLER;
	}
	arena->next_page += arena->page_size;
	return EXCEPTION_CONTINUE_EXECUTION;
}

void* arena_allocate_unaligned(arena_allocator* arena, void* source, size_t size) {
    assert(source != NULL && size != 0);
    void* result = (void*)arena->bump_pointer;
    arena->bump_pointer += size;
    __try {
        memcpy(result, source, size);
    }
    __except (page_fault_handler(arena, GetExceptionCode())) {
        LOG_ERROR("Out of memory!");
        VirtualFree((void*)arena->memory, 0, MEM_RELEASE);
        return NULL;
    }
    return result;
}

inline void *arena_allocate(arena_allocator *arena, void *source, size_t size, size_t alignment)
{
    assert(source != NULL && size != 0);
    size_t mask = alignment - 1;
    arena->bump_pointer = (uintptr_t)(((arena->bump_pointer + mask) & ~mask));
    void* result = (void*)arena->bump_pointer;
    arena->bump_pointer += size;
    __try {
        memcpy(result, source, size);
    }
    __except (page_fault_handler(arena, GetExceptionCode())) {
        LOG_ERROR("Out of memory!");
        VirtualFree((void*)arena->memory, 0, MEM_RELEASE);
        return NULL;
    }
    return result;
}

void destroy_arena_allocator(arena_allocator* arena) {
    VirtualFree((void*)arena->memory, 0, MEM_RELEASE);
}

static LRESULT window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    //CREATESTRUCT* pCreate = (CREATESTRUCT*)(lParam);
    //user_input* input = (user_input*)(pCreate->lpCreateParams);
    user_input* input = (user_input*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCT *pCreate = (CREATESTRUCT*)(lParam);
            user_input* input = (user_input*)(pCreate->lpCreateParams);
            // store the pointer in the instance data of the window
            // so it could always be retrieved by using GetWindowLongPtr(hwnd, GWLP_USERDATA) 
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)input);
        } break;
        case WM_CLOSE: {
            input->closed_window = true;
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_KEYDOWN: {
            int key = (int)wParam;
            if (key < 256) {
                input->keys[key].down = true;
            }
        } break;
        case WM_KEYUP: {
            int key = (int)wParam;
            if (key < 256) {
                input->keys[key].up = true;
            }
        }
        case WM_MOUSEMOVE: {
            input->mouse.x = GET_X_LPARAM(lParam);
            input->mouse.y = GET_Y_LPARAM(lParam);
        };
        case WM_MOUSEWHEEL: {
            input->mouse.wheel = GET_WHEEL_DELTA_WPARAM(wParam);
        } break;
        case WM_LBUTTONDOWN: {
            input->mouse.left_button_down = true;
        } break;
        case WM_LBUTTONUP: {
            input->mouse.left_button_up = true;
        } break;
        case WM_RBUTTONDOWN: {
            input->mouse.right_button_down = true;
        } break;
        case WM_RBUTTONUP: {
            input->mouse.right_button_up = true;
        } break;
        case WM_MBUTTONDOWN: {
            input->mouse.middle_button_down = true;
        } break;
        case WM_MBUTTONUP: {
            input->mouse.middle_button_up = true;
        } break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

// Window creation for platform independent code
result create_window(window* window, const char* title, int width, int height, window_mode mode) {
    memset(window, 0, sizeof(*window));
    if (mode == WINDOW_MODE_FULLSCREEN) {
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
    }
    window->mode = mode;
    window->platform_dependent.instance = GetModuleHandle(NULL);
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = window->platform_dependent.instance;
    wc.lpszClassName = "window_class";
    RegisterClassA(&wc);
    window->platform_dependent.handle = CreateWindowExA(0, wc.lpszClassName, title, mode == WINDOW_MODE_WINDOWED ? WS_OVERLAPPEDWINDOW : WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, window->platform_dependent.instance, &window->input);
    if (window->platform_dependent.handle == NULL) {
        LOG_ERROR("Failed to create window");
        return FAILURE;
    }

    ShowWindow(window->platform_dependent.handle, SW_SHOW);

    POINT point = {0};
    bool is_cursor_pos = GetCursorPos(&point);
    bool is_cursor_within_window = ScreenToClient(window->platform_dependent.handle, &point);
    if (is_cursor_pos && is_cursor_within_window) {
        window->input.mouse.x = point.x;
        window->input.mouse.y = point.y;
    }

    return SUCCESS;
}

void set_window_mode(window* window, window_mode mode) {
    window->mode = mode;
    switch (mode) {
        case WINDOW_MODE_WINDOWED: {
            // Remove window borders and title bar
            SetWindowLong(window->platform_dependent.handle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

            // Resize the window to cover the entire screen
            SetWindowPos(window->platform_dependent.handle, HWND_TOP, 0, 0, 800, 600, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        } break;
        case WINDOW_MODE_BORDERLESS: {
            // Remove window borders and title bar
            SetWindowLong(window->platform_dependent.handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);

            // Resize the window to cover the entire screen
            SetWindowPos(window->platform_dependent.handle, HWND_TOP, 0, 0, 800, 600, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        } break;
        case WINDOW_MODE_FULLSCREEN: {
            // Get the screen width and height
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);

            // Remove window borders and title bar
            SetWindowLong(window->platform_dependent.handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);

            // Resize the window to cover the entire screen
            SetWindowPos(window->platform_dependent.handle, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        } break;
    }
}

void update_window(window *window)
{
    // Clear the input for the next frame
    // All input is cleared except for the mouse position, just in case the mouse is not moved in the next frame.
    int old_mouse_x = window->input.mouse.x;
    int old_mouse_y = window->input.mouse.y;
    memset(&window->input, 0, sizeof(window->input));
    window->input.mouse.x = old_mouse_x;
    window->input.mouse.y = old_mouse_y;

    MSG msg = {0};
    while (PeekMessage(&msg, window->platform_dependent.handle, 0, 0, PM_REMOVE)) {
        //TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void destroy_window(window* window) {
    DestroyWindow(window->platform_dependent.handle);
}

void create_mutex(mutex* mutex) {
    InitializeCriticalSection(&mutex->platform_dependent.critical_section);
}

void lock_mutex(mutex* mutex) {
    EnterCriticalSection(&mutex->platform_dependent.critical_section);
}

void unlock_mutex(mutex* mutex) {
    LeaveCriticalSection(&mutex->platform_dependent.critical_section);
}

void destroy_mutex(mutex* mutex) {
    DeleteCriticalSection(&mutex->platform_dependent.critical_section);
}

result create_thread(thread* thread, unsigned long (*function)(void*), void* data) {
    thread->platform_dependent.handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)function, data, 0, NULL);
    if (thread->platform_dependent.handle == NULL) {
        LOG_ERROR("Failed to create thread");
        return FAILURE;
    }
    return SUCCESS;
}

void destroy_thread(thread* thread) {
    WaitForSingleObject(thread->platform_dependent.handle, INFINITE);
    CloseHandle(thread->platform_dependent.handle);
}

#endif // PLATFORM_IMPLEMENTATION