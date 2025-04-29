#include <iostream>

#define VMA_IMPLEMENTATION
#include "renderer.h"
#include "initializers.h"
#include "tools.h"
#include "shader.h"

#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

#include "../util.h"
#include "../window.h"

#ifdef NDEBUG
constexpr bool USE_VALIDATION_LAYERS = false;
#else
constexpr bool USE_VALIDATION_LAYERS = true;
#endif

namespace {
    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        std::cerr << "validation Layer Error: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
};

VkSwapchainCreateInfoKHR Renderer::get_swapchain_ci(uint32_t width, uint32_t height) {
    return VkSwapchainCreateInfoKHR {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = 2,
        .imageFormat = image_format,
        .imageColorSpace = color_space,
        .imageExtent = {
            .width = width,
            .height = height
        },
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .queueFamilyIndexCount = graphics_queue_family,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR
    };
}

VkImageCreateInfo Renderer::get_render_image_ci(uint32_t width, uint32_t height) {
    return VkImageCreateInfo  {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = image_format,
        .extent = {
            .width = width,
            .height = height, 
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = sample_count,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
}

Renderer::Renderer(Window& window) {
    command_buffers.resize(MAX_FRAMES_IN_FLIGHT);
    fences.resize(MAX_FRAMES_IN_FLIGHT);
    present_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    render_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    i_buffers.resize(MAX_FRAMES_IN_FLIGHT);
    v_buffers.resize(MAX_FRAMES_IN_FLIGHT);

    // Instance
    const char* required_instance_extensions[] = {
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    };
    vkb::InstanceBuilder builder;
    vkb::Result<vkb::Instance> vkb_instance_result = builder.set_app_name("2D-Platformer")
        .request_validation_layers(USE_VALIDATION_LAYERS)
        .enable_extensions(ArrayCount(required_instance_extensions), required_instance_extensions)
        .set_debug_callback(debug_callback)
        .require_api_version(1, 3, 0)
        .build();
    if (!vkb_instance_result.has_value()) {
        std::cerr << "Could not initialize instance with vk-bootstrap" << std::endl;
        exit(-1);
    }
    vkb::Instance vkb_instance = vkb_instance_result.value();
    instance = vkb_instance.instance;
    debug_messenger = vkb_instance.debug_messenger;

    // Surface
    chk_sdl(SDL_Vulkan_CreateSurface(window.raw, instance, nullptr, &surface));

    // Device
    Renderer::enabled_features = {};
    Renderer::enabled_features11 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
    };
    Renderer::enabled_features12 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
    };
    Renderer::enabled_features13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .dynamicRendering = true
    };
    const char* required_device_extensions[] = {
        VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME
    };
    vkb::PhysicalDeviceSelector selector{vkb_instance};
	vkb::Result<vkb::PhysicalDevice> physical_device_result = selector
		.set_minimum_version(1, 3)
        .set_required_features(Renderer::enabled_features)
        .add_required_extensions(ArrayCount(required_device_extensions), required_device_extensions)
		.set_required_features_11(Renderer::enabled_features11)
		.set_required_features_12(Renderer::enabled_features12)
		.set_required_features_13(Renderer::enabled_features13)
		.set_surface(surface)
		.select();
    if (!physical_device_result.has_value()) {
        std::cerr << "Could not initialize physical device with vk-bootstrap" << std::endl;
        exit(-1);
    }
    vkb::PhysicalDevice vkb_physical_device = physical_device_result.value();
    vkb::DeviceBuilder device_builder{vkb_physical_device};
	vkb::Result<vkb::Device> device_result = device_builder.build();
    if (!device_result.has_value()) {
        std::cerr << "Could not initialize device with vk-bootstrap" << std::endl;
        exit(-1);
    }
    vkb::Device vkb_device = device_result.value();
    physical_device = vkb_physical_device.physical_device;
    device = vkb_device.device;
    
    // Store properties/features/etc
    vkGetPhysicalDeviceProperties(physical_device, &properties);
    const auto max_sets = properties.limits.maxBoundDescriptorSets;
    vkGetPhysicalDeviceFeatures(physical_device, &features);
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

    // Create Graphics Queue
    vkb::Result<VkQueue> graphics_queue_result = vkb_device.get_queue(vkb::QueueType::graphics);
    if (!graphics_queue_result.has_value()) {
        std::cerr << "Could not initialize graphics queue with vk-bootstrap" << std::endl;
        exit(-1);
    }
    graphics_queue = graphics_queue_result.value();
	vkb::Result<uint32_t> graphics_queue_family_result = vkb_device.get_queue_index(vkb::QueueType::graphics);
    if (!graphics_queue_family_result.has_value()) {
        std::cerr << "Could not initialize graphics queue family with vk-bootstrap" << std::endl;
        exit(-1);
    }
    graphics_queue_family = graphics_queue_family_result.value();

    // VMA
	VmaVulkanFunctions vk_functions = {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        .vkCreateImage = vkCreateImage
    };
	VmaAllocatorCreateInfo allocator_ci = {
        .physicalDevice = physical_device,
        .device = device,
        .pVulkanFunctions = &vk_functions,
        .instance = instance
    };
	chk(vmaCreateAllocator(&allocator_ci, &allocator));

    // Presentation
    Size2Di32 window_size = window.get_size();
    VkSwapchainCreateInfoKHR swapchain_ci = get_swapchain_ci((uint32_t)window_size.width, (uint32_t)window_size.height);
    chk(vkCreateSwapchainKHR(device, &swapchain_ci, nullptr, &swapchain));
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
    swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());
    swapchain_image_views.resize(image_count);
    VkImageCreateInfo render_image_ci = get_render_image_ci((uint32_t)window_size.width, (uint32_t)window_size.height);
    VmaAllocationCreateInfo alloc_ci = {
        .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.0f
    };
    vmaCreateImage(allocator, &render_image_ci, &alloc_ci, &render_image, &render_image_allocation, nullptr);
    VkImageViewCreateInfo view_ci = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = render_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = image_format,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };
    chk(vkCreateImageView(device, &view_ci, nullptr, &render_image_view));
    for (auto i = 0; i < image_count; i++) {
        view_ci.image = swapchain_images[i];
        chk(vkCreateImageView(device, &view_ci, nullptr, &swapchain_image_views[i]));
    }

    // Command Pool
    VkCommandPoolCreateInfo command_pool_ci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = graphics_queue_family
    };
    chk(vkCreateCommandPool(device, &command_pool_ci, nullptr, &command_pool));

    // Sync objects
    for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkCommandBufferAllocateInfo command_buffer_ai = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, .commandPool = command_pool, .commandBufferCount = 1 };
        chk(vkAllocateCommandBuffers(device, &command_buffer_ai, &command_buffers[i]));
        VkFenceCreateInfo fence_ci = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT };
        vkCreateFence(device, &fence_ci, nullptr, &fences[i]);
        VkSemaphoreCreateInfo semaphore_ci = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        chk(vkCreateSemaphore(device, &semaphore_ci, nullptr, &present_semaphores[i]));
        chk(vkCreateSemaphore(device, &semaphore_ci, nullptr, &render_semaphores[i]));
    }

    // Descriptor Pool
    // TODO: This max_descriptor_count thing sucks, better dyanamic descriptor pool size stuff later,
    // with creating sets with fixed amounts as needed
    const uint32_t max_descriptor_count = 20;
    const VkDescriptorPoolSize pool_sizes[]{
        VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = static_cast<uint32_t>(max_descriptor_count)
        },
        VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = static_cast<uint32_t>(max_descriptor_count)
        }
    };
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = ArrayCount(pool_sizes);
    pool_info.pPoolSizes = &pool_sizes[0];
    pool_info.maxSets = static_cast<uint32_t>(max_descriptor_count * 2);
    chk(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool));


    vkCmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR)vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR");
    if (!vkCmdPushDescriptorSetKHR) {
        std::cout << "Could not get a valid function pointer for vkCmdPushDescriptorSetKHR" << std::endl;
        exit(-1);
    }

    //
    // imgui, move it later
    //

    // Create Imgui-exclusice Descriptor pool
	// NOTE: This pool is huge, but copied from demo
	const VkDescriptorPoolSize imgui_pool_sizes[] = { 
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

	VkDescriptorPoolCreateInfo imgui_pool_info = {};
    imgui_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    imgui_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    imgui_pool_info.maxSets = 1000;
    imgui_pool_info.poolSizeCount = ArrayCount(imgui_pool_sizes);
    imgui_pool_info.pPoolSizes = imgui_pool_sizes;

	chk(vkCreateDescriptorPool(device, &imgui_pool_info, nullptr, &imgui_descriptor_pool));

	// Initialize Imgui library
	ImGui::CreateContext();

    const float scale = SDL_GetWindowDisplayScale(window.raw);
    ImGui::GetStyle().ScaleAllSizes(scale);
    ImGui::GetIO().FontGlobalScale = scale;

	ImGui_ImplSDL3_InitForVulkan(window.raw);

	ImGui_ImplVulkan_InitInfo vulkan_init_info = {};
	vulkan_init_info.Instance = instance;
	vulkan_init_info.PhysicalDevice = physical_device;
	vulkan_init_info.Device = device;
	vulkan_init_info.Queue = graphics_queue;
	vulkan_init_info.DescriptorPool = imgui_descriptor_pool;
	vulkan_init_info.MinImageCount = 3;
	vulkan_init_info.ImageCount = 3;
	vulkan_init_info.UseDynamicRendering = true;

	vulkan_init_info.PipelineRenderingCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO
    };
	vulkan_init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	vulkan_init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &image_format;

	vulkan_init_info.MSAASamples = VK_SAMPLE_COUNT_4_BIT;

	ImGui_ImplVulkan_Init(&vulkan_init_info);

	ImGui_ImplVulkan_CreateFontsTexture();
}

Renderer::~Renderer() {
    vkDeviceWaitIdle(device);
    for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFence(device, fences[i], nullptr);
        vkDestroySemaphore(device, present_semaphores[i], nullptr);
        vkDestroySemaphore(device, render_semaphores[i], nullptr);
    }
    vmaDestroyImage(allocator, render_image, render_image_allocation);
    vkDestroyImageView(device, render_image_view, nullptr);
    for (auto i = 0; i < swapchain_image_views.size(); i++) {
        vkDestroyImageView(device, swapchain_image_views[i], nullptr);
    }
    for (Buffer& v_buffer : v_buffers) {
        v_buffer.destroy(allocator);
    }
    for (Buffer& i_buffer : i_buffers) {
        i_buffer.destroy(allocator);
    }
    for (auto& [id, buffer] : buffers) {
        buffer.destroy(allocator);
    }
    for (auto& [id, texture] : textures) {
        texture.destroy(this);
    }
    for (auto& [id, shader] : shaders) {
        shader.destroy(device);
    }
    vkDestroyCommandPool(device, command_pool, nullptr);
    for (auto& [id, pipeline_layout] : pipeline_layouts) {
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    }
    for (auto& [id, pipeline] : pipelines) {
        pipeline.destroy(device);
    }
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    purgatory.destroy(this);
    vmaDestroyAllocator(allocator);
    vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(device, imgui_descriptor_pool, nullptr);
    for (auto& [id, layout] : descriptor_set_layouts) {
        vkDestroyDescriptorSetLayout(device, layout, nullptr);
    }
    vkDestroyDevice(device, nullptr);
    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
}

bool Renderer::contains_texture(TextureID id) const {
    return textures.contains(id);
}

void Renderer::upload_texture(TextureID id, const TextureCreateInfo& ci) {
    if (!textures.contains(id)) {
        textures.try_emplace(id, this, ci);
    }
}

void Renderer::upload_shader(ShaderID id, const char* path) {
    if (!shaders.contains(id)) {
        const std::vector<uint8_t> shader_bytes = read_file_to_buffer<uint8_t>(path);
        shaders.try_emplace(id, device, shader_bytes.size(), &shader_bytes[0]);
    }
}

DescriptorSetLayoutID Renderer::upload_descriptor_set_layout(std::span<const VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateFlags flags) {
    const DescriptorSetLayoutID id = DescriptorSetLayoutID(descriptor_set_layouts.size());

    VkDescriptorSetLayoutCreateInfo layout_info = initializers::descriptor_set_create_info(bindings, flags);
    VkDescriptorSetLayout layout;
    chk(vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &layout));
    descriptor_set_layouts.emplace(id, layout);

    return id;
}

DescriptorSetID Renderer::upload_descriptor_set(DescriptorSetLayoutID layout_id) {
    VkDescriptorSetLayout& layout = descriptor_set_layouts[layout_id];
    const DescriptorSetID id = DescriptorSetID(descriptor_sets.size());

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;
    VkDescriptorSet set;
    chk(vkAllocateDescriptorSets(device, &alloc_info, &set));
    descriptor_sets.emplace(id, set);

    return id;
}

void Renderer::upload_pipeline(ShaderID vertex_shader_id, ShaderID fragment_shader_id, std::span<const DescriptorSetLayoutID> layout_ids, bool alpha_blending) {
    const PipelineID pipeline_id(vertex_shader_id, fragment_shader_id, alpha_blending);
    if (!pipelines.contains(pipeline_id)) {
        const ShaderData& vert_shader_data = shaders[vertex_shader_id];
        const ShaderData& frag_shader_data = shaders[fragment_shader_id];
    
        // Push constants
        std::vector<VkPushConstantRange> push_constant_ranges;
        vert_shader_data.append_push_constant_ranges(push_constant_ranges);
        frag_shader_data.append_push_constant_ranges(push_constant_ranges);
    
        // Pipeline layout
        VkPipelineLayout& pipeline_layout = pipeline_layouts[pipeline_id];
        std::vector<VkDescriptorSetLayout> layouts = {};
        for (DescriptorSetLayoutID id : layout_ids) {
            layouts.push_back(descriptor_set_layouts[id]);
        }
        VkPipelineLayoutCreateInfo pipeline_layout_ci = initializers::pipeline_layout_create_info(layouts);
        pipeline_layout_ci.pPushConstantRanges = push_constant_ranges.data();
        pipeline_layout_ci.pushConstantRangeCount = push_constant_ranges.size();
        chk(vkCreatePipelineLayout(device, &pipeline_layout_ci, nullptr, &pipeline_layout));
    
        // Pipeline
        pipelines.try_emplace(pipeline_id,
            this,
            pipeline_id,
            sample_count,
            image_format,
            alpha_blending
        );
    }
}

void Renderer::upload_index_data(void* data, uint64_t size_bytes) {
    Buffer& i_buffer = i_buffers[get_frame_index()];

    if (i_buffer.raw == VK_NULL_HANDLE) {
        i_buffer = Buffer(
            allocator,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_AUTO,
            size_bytes
        );
    } else if (i_buffer.size_bytes < size_bytes) {
        purgatory.buffers[get_frame_index()].push_back(i_buffer);

        i_buffer = Buffer(
            allocator,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_AUTO,
            size_bytes
        );
    }

    i_buffer.write_to(data, size_bytes);
}

void Renderer::upload_vertex_data(void* data, uint64_t size_bytes) {
    Buffer& v_buffer = v_buffers[get_frame_index()];

    if (v_buffer.raw == VK_NULL_HANDLE) {
        v_buffer = Buffer(
            allocator,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_AUTO,
            data,
            size_bytes
        );
    } else if (v_buffer.size_bytes < size_bytes) {
        purgatory.buffers[get_frame_index()].push_back(v_buffer);

        v_buffer = Buffer(
            allocator,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_AUTO,
            size_bytes
        );
    }

    v_buffer.write_to(data, size_bytes);
}

void Renderer::resize_swapchain(Window& window) {
    vkDeviceWaitIdle(device);

    Size2Di32 window_size = window.get_size();
    VkSwapchainCreateInfoKHR swapchain_ci = get_swapchain_ci((uint32_t)window_size.width, (uint32_t)window_size.height);
    swapchain_ci.oldSwapchain = swapchain;
    chk(vkCreateSwapchainKHR(device, &swapchain_ci, nullptr, &swapchain));

    auto old_image_count = image_count;
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
    swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());

    vmaDestroyImage(allocator, render_image, render_image_allocation);
    vkDestroyImageView(device, render_image_view, nullptr);

    for (auto i = 0; i < swapchain_image_views.size(); i++) {
        vkDestroyImageView(device, swapchain_image_views[i], nullptr);
    }
    swapchain_image_views.resize(image_count);

    VkImageCreateInfo render_image_ci = get_render_image_ci((uint32_t)window_size.width, (uint32_t)window_size.height);
    VmaAllocationCreateInfo alloc_ci = {
        .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.0f
    };
    chk(vmaCreateImage(allocator, &render_image_ci, &alloc_ci, &render_image, &render_image_allocation, nullptr));
    VkImageViewCreateInfo viewCI = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = render_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = image_format,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };
    chk(vkCreateImageView(device, &viewCI, nullptr, &render_image_view));
    for (auto i = 0; i < image_count; i++) {
        viewCI.image = swapchain_images[i];
        chk(vkCreateImageView(device, &viewCI, nullptr, &swapchain_image_views[i]));
    }
    vkDestroySwapchainKHR(device, swapchain_ci.oldSwapchain, nullptr);
}

void Renderer::wait_for_and_reset_curr_fence() {
    const uint32_t frame_idx = get_frame_index();

    vkWaitForFences(device, 1, &fences[frame_idx], true, UINT64_MAX);
    vkResetFences(device, 1, &fences[frame_idx]);
}

void Renderer::render(Window& window, std::vector<DrawCommand> draws, double frame_dt) {
    const uint32_t frame_idx = get_frame_index();

    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, present_semaphores[frame_idx], VK_NULL_HANDLE, &image_index);

    VkCommandBuffer& cb = command_buffers[frame_idx];
    VkCommandBufferBeginInfo cb_bi{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    vkResetCommandBuffer(cb, 0);
    vkBeginCommandBuffer(cb, &cb_bi);

    VkImageMemoryBarrier barrier0 = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_GENERAL,
        .image = render_image,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
    };
    vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier0);

    const Size2Di32 window_size = window.get_size();
    const VkExtent2D window_extent = VkExtent2D{
        .width = (uint32_t) window_size.width,
        .height = (uint32_t) window_size.height
    };
    const VkClearValue clear = VkClearValue{ .color = { 0.0f, 0.0f, 0.2f, 1.0f } };
    VkRenderingAttachmentInfo color_attachment_info = initializers::rendering_attachment_info(
        render_image_view,
        VK_IMAGE_LAYOUT_GENERAL,
        swapchain_image_views[image_index],
        &clear
    );
    VkRenderingInfo rendering_info = initializers::rendering_info(window_extent, &color_attachment_info);
    vkCmdBeginRendering(cb, &rendering_info);

    VkViewport vp = {
        .x = 0,
        .y = static_cast<float>(window_size.height),
        .width = static_cast<float>(window_size.width),
        .height = -static_cast<float>(window_size.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    vkCmdSetViewport(cb, 0, 1, &vp);
    VkRect2D scissor = {
        .extent = window_extent
    };
    vkCmdSetScissor(cb, 0, 1, &scissor);

    // TODO: This data should probably be in the `DrawCommand`
    VkDeviceSize v_offset = 0;
    vkCmdBindVertexBuffers(cb, 0, 1, &v_buffers[frame_idx].raw, &v_offset);
    vkCmdBindIndexBuffer(cb, i_buffers[frame_idx].raw, 0, VK_INDEX_TYPE_UINT32);

    uint32_t draw_calls = 0;
    uint32_t tris_drawn = 0;
    uint32_t pipelines_bound = 0;
    VkPipeline prev_pipeline = VK_NULL_HANDLE;
    uint32_t descriptor_sets_bound = 0;
    std::array<DescriptorSetID, MAX_NUM_DESCRIPTOR_SETS> bound_sets = invalid_descriptor_set_ids();
    for (const DrawCommand& draw : draws) {
        const PipelineID pipeline_id(draw.vertex_id, draw.fragment_id, draw.alpha_blending); 
        const Pipeline& pipeline = pipelines[pipeline_id];
        const VkPipelineLayout& pipeline_layout = pipeline_layouts[pipeline_id];

        if (pipeline.raw != prev_pipeline) {
            vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.raw);
            ++pipelines_bound;
            prev_pipeline = pipeline.raw;
            bound_sets = invalid_descriptor_set_ids();
        }

        // descriptors
        for (uint32_t i = 0; i < draw.set_ids.size(); ++i) {
            const DescriptorSetID set_id = draw.set_ids[i];
            if ((set_id != DescriptorSetID::InvalidID()) && (bound_sets[i] != set_id)) {
                vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &descriptor_sets.at(set_id), 0, nullptr);
                ++descriptor_sets_bound;
                bound_sets[i] = set_id;
            }
        }

        // Push descriptors
        std::vector<VkWriteDescriptorSet> writes{};
        for (const PushDescriptorSetData& set_data : draw.push_set_data) {
            VkWriteDescriptorSet write = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = 0,
				.dstBinding = set_data.binding,
				.descriptorCount = 1,
				.descriptorType = set_data.ty,
            };

            if (set_data.ty == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                const Buffer& buffer = buffers[set_data.buffer_id];
                write.pBufferInfo = &buffer.descriptor;
            } else if (set_data.ty == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                const Texture& texture = textures[set_data.texture_id];
                write.pImageInfo = &texture.descriptor;
            }

            writes.push_back(write);
        }
        if (!writes.empty()) {
            vkCmdPushDescriptorSetKHR(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, draw.push_set_idx, writes.size(), writes.data());
        }

        // Push constants
        for (const PushConstantData& pc : draw.pcs) {
            vkCmdPushConstants(cb, pipeline_layout, pc.stage_flags, pc.offset, pc.size, pc.p_data);
        }

        vkCmdDrawIndexed(cb, draw.index_count, 1, draw.first_index, 0, 0);
        ++draw_calls;
        tris_drawn += draw.index_count / 3;
    }

    vkCmdEndRendering(cb);

    //
    // Imgui
    //
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Debug Data");
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Engine Data")) {
            const uint32_t fps = 1.0 / frame_dt;
            ImGui::Text("Frame dt %.3f ms (%u FPS)", frame_dt * 1000, fps);
            ImGui::Text("Draw calls: %u", draw_calls);
            ImGui::Text("Triangles drawn: %u", tris_drawn);
            ImGui::Text("Pipelines bound: %u", pipelines_bound);
            ImGui::Text("DescriptorSets bound: %u", descriptor_sets_bound);
            
            ImGui::TreePop();
        }

        ImGui::Separator();
        imgui_fn();
    }
    ImGui::End();
    ImGui::Render();

    color_attachment_info = initializers::rendering_attachment_info(render_image_view, VK_IMAGE_LAYOUT_GENERAL, swapchain_image_views[image_index]);
	rendering_info = initializers::rendering_info(window_extent, &color_attachment_info);
    vkCmdBeginRendering(cb, &rendering_info);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb);
    vkCmdEndRendering(cb);
    //
    //
    //

    VkImageMemoryBarrier barrier1 = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = 0,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .image = swapchain_images[image_index],
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };
    vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier1);

    vkEndCommandBuffer(cb);

    VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &present_semaphores[frame_idx],
        .pWaitDstStageMask = &wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &cb,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &render_semaphores[frame_idx],
    };
    vkQueueSubmit(graphics_queue, 1, &submit_info, fences[frame_idx]);
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &render_semaphores[frame_idx],
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_index
    };
    chk(vkQueuePresentKHR(graphics_queue, &present_info));

    ++frame_count;
}

VkCommandBuffer Renderer::create_command_buffer(VkCommandBufferLevel level, bool begin, VkQueueFlagBits queue_ty)
{
    VkCommandPool cmd_pool = command_pool;
    // TODO: transfer

    VkCommandBufferAllocateInfo cmd_buff_allocate_info =
        initializers::command_buffer_allocate_info(cmd_pool, level, 1);

    VkCommandBuffer cmd_buffer = VK_NULL_HANDLE;
    chk(vkAllocateCommandBuffers(device, &cmd_buff_allocate_info, &cmd_buffer));

    if (begin) {
        VkCommandBufferBeginInfo cmd_buf_info = initializers::command_buffer_begin_info();
        chk(vkBeginCommandBuffer(cmd_buffer, &cmd_buf_info));
    }

    return cmd_buffer;
}

void Renderer::flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free, VkQueueFlagBits queue_type)
{
    if (command_buffer == VK_NULL_HANDLE) {
        return;
    }

    chk(vkEndCommandBuffer(command_buffer));

    VkSubmitInfo submit_info = initializers::submit_info();
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    VkFenceCreateInfo fence_info = initializers::fence_create_info(VK_FLAGS_NONE);
    VkFence fence;
    chk(vkCreateFence(device, &fence_info, nullptr, &fence));

    chk(vkQueueSubmit(queue, 1, &submit_info, fence));
    chk(vkWaitForFences(device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

    vkDestroyFence(device, fence, nullptr);

    if (free) {
        VkCommandPool cmd_pool = command_pool;
        // TODO: transfer
        vkFreeCommandBuffers(device, cmd_pool, 1, &command_buffer);
    }
}

BufferID Renderer::request_buffer(VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_flags, VmaMemoryUsage vma_usage, uint64_t size_bytes)  {
    const BufferID id = BufferID(buffers.size());
    buffers.try_emplace(id, allocator, usage, allocation_flags, vma_usage, size_bytes);
    return id;
}

Buffer& Renderer::get_buffer(BufferID id) {
    return buffers[id];
}

void Renderer::process_sdl_event(const SDL_Event* e) {
    ImGui_ImplSDL3_ProcessEvent(e);
}