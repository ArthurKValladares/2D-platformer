#pragma once

#include <vector>
#include <unordered_map>

#include <vma/vk_mem_alloc.h>
#include <SDL3/SDL.h>

#include "buffer.h"
#include "texture.h"
#include "draw.h"
#include "pipeline.h"
#include "resource_ids.h"

#define MAX_FRAMES_IN_FLIGHT 2

struct ImguiData {
    double frame_dt;
};

struct Window;
struct Renderer {
    Renderer(Window& window);
    ~Renderer();

    // TODO: the public API is still a bit messy wiht all the create/upload/request functions.
    // standardize all that at some point

    // TODO: Efficient way to do this in a batch
    bool contains_texture(TextureID id) const;
    void upload_texture(TextureID id, const TextureCreateInfo& texture_cis);

    void upload_shader(ShaderID id, const char* path);
    void upload_pipeline(ShaderID vertex_shader_id, ShaderID fragment_shader_id);

    void upload_index_data(void* data, uint64_t size_bytes);
    void upload_vertex_data(void* data, uint64_t size_bytes);

    void resize_swapchain(Window& window);

    void render(Window& window, std::vector<DrawCommand> draws);

    VkQueue get_graphics_queue() {
        return graphics_queue;
    }
    VkDevice get_device() const {
        return device;
    }
    VkDescriptorPool get_descriptor_pool() {
        return descriptor_pool;
    }
    VmaAllocator get_allocator() {
        return allocator;
    }

    const ShaderData& get_shader_data(ShaderID shader_id) const {
        return shaders.at(shader_id);
    }
    const VkPipelineLayout& get_pipeline_layout(PipelineID pipeline_id) const {
        return pipeline_layouts.at(pipeline_id);
    }

    VkCommandBuffer create_command_buffer(VkCommandBufferLevel level, bool begin = false, VkQueueFlagBits queue_ty = VK_QUEUE_GRAPHICS_BIT);
    void flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free = true, VkQueueFlagBits queue_type = VK_QUEUE_GRAPHICS_BIT);
    
    BufferID request_buffer(VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_flags, VmaMemoryUsage vma_usage, uint64_t size_bytes);
    Buffer& get_buffer(BufferID id);

    void process_sdl_event(const SDL_Event* e);
    void setup_imgui_draw(const ImguiData& data);
private:
    VkSwapchainCreateInfoKHR get_swapchain_ci(uint32_t width, uint32_t height);
    VkImageCreateInfo get_render_image_ci(uint32_t width, uint32_t height);

    inline static VkPhysicalDeviceFeatures enabled_features{};
	inline static VkPhysicalDeviceVulkan11Features enabled_features11{};
	inline static VkPhysicalDeviceVulkan12Features enabled_features12{};
	inline static VkPhysicalDeviceVulkan13Features enabled_features13{};
    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR{ VK_NULL_HANDLE };
    
    const VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_4_BIT;
    uint32_t image_index = 0;
    uint32_t frame_count = 0;

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

    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    VkDescriptorPool imgui_descriptor_pool = VK_NULL_HANDLE;

    std::unordered_map<BufferID, Buffer> buffers;
    std::unordered_map<TextureID, Texture> textures;
    std::unordered_map<ShaderID, ShaderData> shaders;
    std::unordered_map<PipelineID, std::vector<VkDescriptorSetLayout>> descriptor_set_layouts;
    std::unordered_map<PipelineID, VkPipelineLayout> pipeline_layouts;
    std::unordered_map<PipelineID, Pipeline> pipelines;

    friend class Texture;
};