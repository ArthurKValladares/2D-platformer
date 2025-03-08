#pragma once

#include <vector>

#include <vma/vk_mem_alloc.h>

#define MAX_FRAMES_IN_FLIGHT 2

struct Buffer {
    Buffer() {}

    template<class T>
    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, const std::vector<T>& data) {
        const uint64_t size_bytes = data.size() * sizeof(T);
        VkBufferCreateInfo buffer_ci = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size_bytes,
            .usage = usage
        };
        VmaAllocationCreateInfo buffer_alloc_ci = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };
        chk(vmaCreateBuffer(allocator, &buffer_ci, &buffer_alloc_ci, &raw, &allocation, nullptr));
        void* buffer_ptr = nullptr;
        vmaMapMemory(allocator, allocation, &buffer_ptr);
        memcpy(buffer_ptr, (void*) data.data(), size_bytes);
        vmaUnmapMemory(allocator, allocation);

        length = data.size();
    }

    void destroy(VmaAllocator allocator);

    VmaAllocation allocation = VK_NULL_HANDLE;
    VkBuffer raw = VK_NULL_HANDLE;
    uint64_t size_bytes = 0;
    uint64_t length = 0;
};

struct Window;
struct VkRenderer {
    VkRenderer(Window& window);
    ~VkRenderer();

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