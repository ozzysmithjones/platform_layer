#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NO_MINMAX
#include <Windows.h>
#include <windowsx.h>
#endif
#include "platform.h"

IMPLEMENT_CAPPED_ARRAY(wchar_t, typed_characters, MAX_TYPED_CHARACTERS)

const char* window_class_name = "MyWindowClass";

static LRESULT window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    user_input* input = (user_input*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_CREATE:
        CREATESTRUCT* pCreate = (CREATESTRUCT*)(lParam);
        input = (user_input*)(pCreate->lpCreateParams);
        // store the pointer in the instance data of the window
        // so it could always be retrieved by using GetWindowLongPtr(hwnd, GWLP_USERDATA) 
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)input);
        break;
    case WM_CLOSE: {
        input->closed_window = true;
        return 0;
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
    case WM_CHAR: {
        wchar_t character = (wchar_t)wParam;
        if (character != 0) {
            typed_characters_append(&input->typed_characters, character);
        }
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
    } break;
    case WM_MOUSEMOVE: {
        input->mouse.x = GET_X_LPARAM(lParam);
        input->mouse.y = GET_Y_LPARAM(lParam);
    } break;
    case WM_MOUSEWHEEL: {
        input->mouse.scroll_delta = GET_WHEEL_DELTA_WPARAM(wParam);
    } break;
    case WM_LBUTTONDOWN: {
        input->keys[KEY_LEFT_MOUSE].down = true;
    } break;
    case WM_LBUTTONUP: {
        input->keys[KEY_LEFT_MOUSE].up = true;
    } break;
    case WM_RBUTTONDOWN: {
        input->keys[KEY_RIGHT_MOUSE].down = true;
    } break;
    case WM_RBUTTONUP: {
        input->keys[KEY_RIGHT_MOUSE].up = true;
    } break;
    case WM_MBUTTONDOWN: {
        input->keys[KEY_MIDDLE_MOUSE].down = true;
    } break;
    case WM_MBUTTONUP: {
        input->keys[KEY_MIDDLE_MOUSE].up = true;
    } break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

STATIC_ASSERT(sizeof(void*) == sizeof(HANDLE), window_handle_size_mismatch);

result create_window(const char* title, uint32_t width, uint32_t height, window_mode mode, window* out_window) {
    if (!out_window) {
        ERROR_BREAKPOINT("Window pointer is null");
        return RESULT_FAILURE;
    }

    memset(out_window, 0, sizeof(window));

    int32_t style = 0;
    if (mode == WINDOW_MODE_FULLSCREEN) {
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        style = WS_POPUP;
    }
    else if (mode == WINDOW_MODE_BORDERLESS) {
        style = WS_POPUP | WS_BORDER;
    }
    else {
        DEBUG_ASSERT(mode == WINDOW_MODE_WINDOWED, , "Window mode not supported");
        style = WS_OVERLAPPEDWINDOW;
    }

    // Create the window here
    static bool registered_class = false;

    if (!registered_class) {
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = window_proc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = window_class_name;
        RegisterClass(&wc);
        registered_class = true;
    }

    HWND hwnd = CreateWindowEx(
        0,
        window_class_name,
        title,
        style,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        &out_window->input
    );

    if (!hwnd) {
        ERROR_BREAKPOINT("Failed to create window");
        return RESULT_FAILURE;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    POINT point = { 0 };
    bool is_cursor_pos = GetCursorPos(&point);
    bool is_cursor_within_window = ScreenToClient(out_window->handle, &point);
    if (is_cursor_pos && is_cursor_within_window) {
        out_window->input.mouse.x = point.x;
        out_window->input.mouse.y = point.y;
    }

    out_window->handle = hwnd;
    return RESULT_SUCCESS;
}

void update_window_input(window* window) {
    if (window == NULL) {
        ERROR_BREAKPOINT("Window pointer is null");
        return;
    }

    if (window->handle == NULL) {
        ERROR_BREAKPOINT("Window handle is null");
        return;
    }

    int32_t old_cursor_x = window->input.mouse.x;
    int32_t old_cursor_y = window->input.mouse.y;
    memset(&window->input, 0, sizeof(window->input));
    window->input.mouse.x = old_cursor_x;
    window->input.mouse.y = old_cursor_y;

    MSG msg = { 0 };
    while (PeekMessage(&msg, window->handle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void destroy_window(window* window) {
    if (window == NULL) {
        ERROR_BREAKPOINT("Window pointer is null");
        return;
    }

    if (window->handle != NULL) {
        DestroyWindow(window->handle);
        window->handle = NULL;
    }
}