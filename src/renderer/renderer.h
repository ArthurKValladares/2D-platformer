#pragma once

#include <vector>

#include <vma/vk_mem_alloc.h>

#define MAX_FRAMES_IN_FLIGHT 2

#include "buffer.h"

struct Window;
struct Renderer {
    Renderer(Window& window);
    ~Renderer();

    void resize_swapchain(Window& window);

    void render(Window& window);

private:
    VkSwapchainCreateInfoKHR get_swapchain_ci(uint32_t width, uint32_t height);
    VkImageCreateInfo get_render_image_ci(uint32_t width, uint32_t height);

    const VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_4_BIT;
    uint32_t image_index = 0;
    uint32_t frame_index = 0;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t q_family_idx = 0;
    VkQueue queue = VK_NULL_HANDLE;

    VmaAllocator allocator = VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    uint32_t image_count;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;

    const VkFormat image_format = VK_FORMAT_B8G8R8A8_SRGB;
    const VkColorSpaceKHR color_space = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

    VkImage render_image;
    VmaAllocation render_image_allocation;
    VkImageView render_image_view;

    Buffer v_buffer;

    Buffer i_buffer;

    VkCommandPool command_pool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> command_buffers;
    std::vector<VkFence> fences;
    std::vector<VkSemaphore> present_semaphores;
    std::vector<VkSemaphore> render_semaphores;

    VkPipeline pipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
};