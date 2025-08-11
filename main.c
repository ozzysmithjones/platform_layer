#include "fundamental.h"
#include "platform.h"
#include "graphics.h"

static window main_window;
int main() {
    create_window("Main Window", 800, 600, WINDOW_MODE_WINDOWED, &main_window);
    renderer main_renderer = { 0 };
    create_renderer(&main_window, &main_renderer);

    while (!main_window.input.closed_window) {
        update_window_input(&main_window);
    }
    destroy_renderer(&main_renderer);
    destroy_window(&main_window);
    return 0;
}
