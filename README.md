# platform_layer
This is a header-only C library that wraps the native Windows API. Other platforms might be supported in the future. This allows native access to keyboard, windowing, multi-threading, file mapping, dynamic library loading and memory management for desktop applications. 

**Warning**
This library is still in its early stages

# integrating the library
Copy the platform_layer.h file into your C/C++ project. In one of the .c/.cpp files within your project, type the following to implement the library:

```C
#define PLATFORM_IMPLEMENTATION
#include "platform_layer.h"
```

The #define PLATFORM_IMPLEMENTATION is a macro that should only be defined in exactly one .c/.cpp file. This macro is used so that the library functions can be implemented one time (if there's multiple implementations of a function this can lead to linker errors in C/C++). After doing this in one file, you are free to #include platform.h in other files, provided there is one implementation. 

# usage details
Every function that begins with 'create' should be proceeded with a following call to 'destroy' to cleanup system resources (similar to malloc/free). For example, `create_window()` should be followed by `destroy_window()` later on in the program. Here is an example of how to create/destroy a window: 

```C
#define PLATFORM_IMPLEMENTATION
#include "platform_layer.h"

int main(void) {
    window window = { 0 };
    if (create_window(&window, "Test", 800, 600, WINDOW_MODE_WINDOWED) != SUCCESS) {
        return;
    }

    while (!window.input.closed_window) {
        update_window(&window);
        if (window.input.keys[KEY_ESCAPE].down) {
            break;
        }
    }
    
    destroy_window(&window);
    return 0;
}
```
Some functions can fail and will return an error code. Error codes can be either `SUCCESS` or `FAILURE`. In addition to returning the error code, in debug mode functions will also log an error message to standard error when an error occurs. This behaviour can be overriden by defining the LOG_ERROR macro before implementing the library. 
```C
#define LOG_ERROR(message) printf("this is a nice error message: %s", message)
#define PLATFORM_IMPLEMENTATION
#include "platform_layer.h"
```
