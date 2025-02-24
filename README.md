# platform_layer
This is a header-only library that abstracts over the native Windows API. Other platforms might be supported in the future. This allows native access to keyboard, windowing, multi-threading, file mapping, dynamic library loading and memory management for desktop applications. 

# how to use
Copy the platform_layer.h file into your C/C++ project. In one of the .c/.cpp files within your project, type the following:

```C
#define PLATFORM_IMPLEMENTATION
#include "platform_layer.h"
```

The #define PLATFORM_IMPLEMENTATION is a macro that should only be defined in exactly one .c/.cpp file. This macro is so that the library functions are only implemented one time (if there's multiple implementations of a function this can lead to linker errors in C/C++). After doing this in one file, you are free to use #include platform.h in other files, provided there is one implementation. 
