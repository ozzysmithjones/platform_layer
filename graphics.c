#include "graphics.h"
#if defined (PLATFORM_WINDOWS)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#define REQUIRED_DEVICE_EXTENSION_COUNT  1
const char* required_device_extensions[REQUIRED_DEVICE_EXTENSION_COUNT] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

#define REQUIRED_VALIDATION_LAYER_COUNT  1
const char* required_validation_layers[REQUIRED_VALIDATION_LAYER_COUNT] = {
    "VK_LAYER_KHRONOS_validation",
};

#define REQUIRED_SURFACE_EXTENSION_COUNT 2
const char* required_surface_extensions[REQUIRED_SURFACE_EXTENSION_COUNT] = {
#ifdef PLATFORM_WINDOWS
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else
    #error "Unsupported platform for Vulkan surface extensions."
#endif
    VK_KHR_SURFACE_EXTENSION_NAME,
};

typedef struct {
    uint32_t graphics_queue_index;
    uint32_t transfer_queue_index;
} queue_families;

static VkInstance create_vulkan_instance(void) {
    VkInstance instance;
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Yggdrasil",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Yggdrasil Game Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = REQUIRED_SURFACE_EXTENSION_COUNT,
        .ppEnabledExtensionNames = required_surface_extensions,
    };

#ifdef NDEBUG
    // Release build: disable validation layers
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = NULL;
#else
    // Debug build: enable validation layers
    create_info.enabledLayerCount = REQUIRED_VALIDATION_LAYER_COUNT;
    create_info.ppEnabledLayerNames = required_validation_layers;
#endif

    VkResult result = vkCreateInstance(&create_info, NULL, &instance);
    if (result != VK_SUCCESS) {
        ERROR_BREAKPOINT("Failed to create Vulkan instance.");
        return VK_NULL_HANDLE;
    }

    return instance;
}

static VkSurfaceKHR create_window_surface(window* window, VkInstance instance) {
    ASSERT(window != NULL, return VK_NULL_HANDLE, "Window pointer is NULL");
    ASSERT(instance != VK_NULL_HANDLE, return VK_NULL_HANDLE, "Vulkan instance is NULL");

    VkSurfaceKHR surface = VK_NULL_HANDLE;

#if defined (PLATFORM_WINDOWS)
    VkResult result = vkCreateWin32SurfaceKHR(instance, &(VkWin32SurfaceCreateInfoKHR){
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .hinstance = GetModuleHandle(NULL),
            .hwnd = window->handle,
    }, NULL, & surface);
#else
#error "Unsupported platform for Vulkan surface creation."
#endif

    if (result != VK_SUCCESS) {
        ERROR_BREAKPOINT("Failed to create Vulkan surface.");
    }

    return surface;
}

static bool has_required_queue_family_details(VkPhysicalDevice device, VkSurfaceKHR window_surface, queue_families* out_queue_families) {
    ASSERT(device != VK_NULL_HANDLE, return false, "Physical device is NULL");
    ASSERT(window_surface != VK_NULL_HANDLE, return false, "Window surface is NULL");
    ASSERT(out_queue_families != NULL, return false, "Output queue families structure is NULL");

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
    if (queue_family_count == 0) {
        return false;
    }

    VkQueueFamilyProperties* queue_families = alloca(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    bool graphics_found = false;
    bool transfer_found = false;

    for (uint32_t i = 0; i < queue_family_count; ++i) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            // Also check for present support on the graphics family:
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, window_surface, &present_support);
            if (present_support) {
                out_queue_families->graphics_queue_index = i;
                graphics_found = true;
                continue;
            }
        }

        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            out_queue_families->transfer_queue_index = i;
            transfer_found = true;
        }
    }

    if (!transfer_found && graphics_found) {
        out_queue_families->transfer_queue_index = out_queue_families->graphics_queue_index;
        transfer_found = true;
    }

    return graphics_found && transfer_found;
}

static bool has_required_extensions(VkPhysicalDevice device) {
    ASSERT(device != VK_NULL_HANDLE, return false, "Physical device is NULL");
    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
    if (extension_count < REQUIRED_DEVICE_EXTENSION_COUNT) {
        return false;
    }

    VkExtensionProperties* extensions = alloca(sizeof(VkExtensionProperties) * extension_count);
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, extensions);

    for (uint32_t i = 0; i < REQUIRED_DEVICE_EXTENSION_COUNT; ++i) {
        bool found = false;
        for (uint32_t j = 0; j < extension_count; ++j) {
            if (strcmp(extensions[j].extensionName, required_device_extensions[i]) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}

static VkPhysicalDevice pick_physical_device(VkSurfaceKHR window_surface, VkInstance instance, queue_families* out_queue_families) {
    ASSERT(window_surface != VK_NULL_HANDLE, return VK_NULL_HANDLE, "Window surface is NULL");
    ASSERT(instance != VK_NULL_HANDLE, return VK_NULL_HANDLE, "Vulkan instance is NULL");
    ASSERT(out_queue_families != NULL, return VK_NULL_HANDLE, "Output queue families structure is NULL");

    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    if (device_count == 0) {
        ERROR_BREAKPOINT("No Vulkan-compatible devices found.");
        return VK_NULL_HANDLE;
    }

    VkPhysicalDevice* devices = alloca(sizeof(VkPhysicalDevice) * device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    VkPhysicalDevice best_device = VK_NULL_HANDLE;
    uint32_t score = 0;

    for (uint32_t i = 0; i < device_count; ++i) {
        if (!has_required_extensions(devices[i])) {
            continue;
        }

        queue_families queue_families = { 0 };
        if (!has_required_queue_family_details(devices[i], window_surface, &queue_families)) {
            continue;
        }

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[i], &properties);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            memcpy(out_queue_families, &queue_families, sizeof(queue_families));
            best_device = devices[i];
            break;
        }

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && score < 3) {
            memcpy(out_queue_families, &queue_families, sizeof(queue_families));
            best_device = devices[i];
            score = 3;
            continue;
        }

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU && score < 2) {
            memcpy(out_queue_families, &queue_families, sizeof(queue_families));
            best_device = devices[i];
            score = 2;
            continue;
        }

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU && score < 1) {
            memcpy(out_queue_families, &queue_families, sizeof(queue_families));
            best_device = devices[i];
            score = 1;
            continue;
        }

        if (best_device == VK_NULL_HANDLE) {
            memcpy(out_queue_families, &queue_families, sizeof(queue_families));
            best_device = devices[i];
            score = 0;
        }
    }

    if (best_device == VK_NULL_HANDLE) {
        ERROR_BREAKPOINT("No suitable Vulkan physical device found.");
        return VK_NULL_HANDLE;
    }

    return best_device;
}

static VkDevice create_logical_device(VkPhysicalDevice physical_device, queue_families queue_families, VkQueue* out_graphics_queue, VkQueue* out_transfer_queue) {
    ASSERT(physical_device != VK_NULL_HANDLE, return VK_NULL_HANDLE, "Physical device is NULL");
    ASSERT(out_graphics_queue != NULL, return VK_NULL_HANDLE, "Output graphics queue pointer is NULL");
    ASSERT(out_transfer_queue != NULL, return VK_NULL_HANDLE, "Output transfer queue pointer is NULL");

    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info[2] = { 0 };
    uint32_t queue_count = 0;

    if (queue_families.graphics_queue_index != UINT32_MAX) {
        queue_create_info[queue_count++] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_families.graphics_queue_index,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
        };
    }

    if (queue_families.transfer_queue_index != UINT32_MAX && queue_families.transfer_queue_index != queue_families.graphics_queue_index) {
        queue_create_info[queue_count++] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_families.transfer_queue_index,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
        };
    }

    VkPhysicalDeviceFeatures device_features = { 0 };

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queue_create_info,
        .queueCreateInfoCount = queue_count,
        .pEnabledFeatures = &device_features,
        .enabledExtensionCount = REQUIRED_DEVICE_EXTENSION_COUNT,
        .ppEnabledExtensionNames = required_device_extensions,
    };

    VkDevice device = VK_NULL_HANDLE;
    if (vkCreateDevice(physical_device, &create_info, NULL, &device) != VK_SUCCESS) {
        ERROR_BREAKPOINT("Failed to create Vulkan logical device.");
        return VK_NULL_HANDLE;
    }

    *out_graphics_queue = VK_NULL_HANDLE;
    *out_transfer_queue = VK_NULL_HANDLE;

    vkGetDeviceQueue(device, queue_families.graphics_queue_index, 0, out_graphics_queue);
    vkGetDeviceQueue(device, queue_families.transfer_queue_index, 0, out_transfer_queue);

    return device;
}

result create_renderer(window* window, renderer* out_renderer) {
    ASSERT(window != NULL, return RESULT_FAILURE, "Window pointer is NULL");
    ASSERT(out_renderer != NULL, return RESULT_FAILURE, "Renderer pointer is NULL");
    memset(out_renderer, 0, sizeof(renderer));
    out_renderer->instance = create_vulkan_instance();
    if (out_renderer->instance == VK_NULL_HANDLE) {
        return RESULT_FAILURE;
    }

    out_renderer->window_surface = create_window_surface(window, out_renderer->instance);
    if (out_renderer->window_surface == VK_NULL_HANDLE) {
        return RESULT_FAILURE;
    }

    queue_families queue_families = { UINT32_MAX, UINT32_MAX };
    out_renderer->physical_device = pick_physical_device(out_renderer->window_surface, out_renderer->instance, &queue_families);
    if (out_renderer->physical_device == VK_NULL_HANDLE) {
        return RESULT_FAILURE;
    }

    out_renderer->graphics_queue_family_index = queue_families.graphics_queue_index;
    out_renderer->transfer_queue_family_index = queue_families.transfer_queue_index;
    out_renderer->device = create_logical_device(out_renderer->physical_device, queue_families, &out_renderer->graphics_queue, &out_renderer->transfer_queue);
    if (out_renderer->device == VK_NULL_HANDLE) {
        return RESULT_FAILURE;
    }

    return RESULT_SUCCESS;
}

void destroy_renderer(renderer* renderer) {
    ASSERT(renderer != NULL, return, "Renderer pointer is NULL");

    vkDestroySurfaceKHR(renderer->instance, renderer->window_surface, NULL);
    vkDestroyDevice(renderer->device, NULL);
    vkDestroyInstance(renderer->instance, NULL);
}

