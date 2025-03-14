#pragma once

#include <vector>

#include <vma/vk_mem_alloc.h>

#define MAX_FRAMES_IN_FLIGHT 2

#include "buffer.h"
#include "texture.h"
#include "draw.h"
#include "material.h"
#include "pipeline.h"

struct Window;
struct Renderer {
    Renderer(Window& window);
    ~Renderer();

    void upload_textures(const std::vector<TextureCreateInfo>& texture_cis);
    void upload_index_data(void* data, uint64_t size_bytes);
    void upload_vertex_data(void* data, uint64_t size_bytes);

    void resize_swapchain(Window& window);

    void render(Window& window, std::vector<DrawCommand> draws);

    VkQueue get_graphics_queue() {
        return graphics_queue;
    }
    VkDevice get_device() {
        return device;
    }
    VkDescriptorPool get_descriptor_pool() {
        return descriptor_pool;
    }

    VkCommandBuffer create_command_buffer(VkCommandBufferLevel level, bool begin = false, VkQueueFlagBits queue_ty = VK_QUEUE_GRAPHICS_BIT);
    void flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free = true, VkQueueFlagBits queue_type = VK_QUEUE_GRAPHICS_BIT);

    inline static VkPhysicalDeviceFeatures enabled_features{};
	inline static VkPhysicalDeviceVulkan11Features enabled_features11{};
	inline static VkPhysicalDeviceVulkan12Features enabled_features12{};
	inline static VkPhysicalDeviceVulkan13Features enabled_features13{};
    
private:
    VkSwapchainCreateInfoKHR get_swapchain_ci(uint32_t width, uint32_t height);
    VkImageCreateInfo get_render_image_ci(uint32_t width, uint32_t height);

    const VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_4_BIT;
    uint32_t image_index = 0;
    uint32_t frame_index = 0;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};
    VkPhysicalDeviceMemoryProperties memory_properties{};
    VkDevice device = VK_NULL_HANDLE;
    uint32_t graphics_queue_family;
    VkQueue graphics_queue = VK_NULL_HANDLE;

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

    // TODO: Dedicated transfer pool
    VkCommandPool command_pool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> command_buffers;
    std::vector<VkFence> fences;
    std::vector<VkSemaphore> present_semaphores;
    std::vector<VkSemaphore> render_semaphores;
    
    std::vector<Texture> textures;
    
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayout> descriptor_set_layouts;

    std::vector<Material> materials;

    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    Pipeline pipeline;

    friend class Texture;
};