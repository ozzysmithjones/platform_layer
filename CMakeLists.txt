cmake_minimum_required(VERSION 3.10.0)
project(platform_layer VERSION 0.1.0 LANGUAGES C)

add_executable(platform_layer main.c platform.c graphics.c)

# Find Vulkan SDK using environment variable
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable not set. Please install Vulkan SDK.")
endif()

# Add Vulkan SDK include directories
include_directories($ENV{VULKAN_SDK}/Include)

# Link statically with Vulkan
if(WIN32)
    target_link_libraries(platform_layer PRIVATE $ENV{VULKAN_SDK}/Lib/vulkan-1.lib)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(platform_layer PRIVATE $ENV{VULKAN_SDK}/lib/libvulkan.a)
elseif(APPLE)
    target_link_libraries(platform_layer PRIVATE $ENV{VULKAN_SDK}/lib/libvulkan.a)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()


