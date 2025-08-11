#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vulkan/vulkan.h>
#include "fundamental.h"
#include "platform.h"

typedef struct {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkSurfaceKHR window_surface;

    uint32_t graphics_queue_family_index;
    uint32_t transfer_queue_family_index;
    VkQueue graphics_queue;
    VkQueue transfer_queue;

    VkSwapchainKHR swapchain;
    VkRenderPass render_pass;
    VkPipeline graphics_pipeline;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkFramebuffer framebuffer;
} renderer;

result create_renderer(window* window, renderer* out_renderer);
void destroy_renderer(renderer* renderer);

#endif // GRAPHICS_H