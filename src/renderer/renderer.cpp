#include <iostream>

#define VMA_IMPLEMENTATION
#include "renderer.h"
#include "initializers.h"
#include "tools.h"
#include "shader.h"

#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>

#include "../util.h"
#include "../window.h"

#ifdef NDEBUG
constexpr bool USE_VALIDATION_LAYERS = false;
#else
constexpr bool USE_VALIDATION_LAYERS = true;
#endif

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

    // Instance
    vkb::InstanceBuilder builder;
    vkb::Result<vkb::Instance> vkb_instance_result = builder.set_app_name("2D-Platformer")
        .request_validation_layers(USE_VALIDATION_LAYERS)
        .use_default_debug_messenger()
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
    Renderer::enabled_features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    Renderer::enabled_features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    Renderer::enabled_features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    vkb::PhysicalDeviceSelector selector{vkb_instance};
	vkb::Result<vkb::PhysicalDevice> physical_device_result = selector
		.set_minimum_version(1, 3)
        .set_required_features(Renderer::enabled_features)
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
    // TODO: This max_image_count thing sucks, better dyanamic descriptor pool size stuff later,
    // with creating sets with fixed amounts as needed
    const uint32_t max_image_count = 20;
    const VkDescriptorPoolSize pool_sizes[]{
        VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = static_cast<uint32_t>(max_image_count)
        }
    };
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = ArrayCount(pool_sizes);
    pool_info.pPoolSizes = &pool_sizes[0];
    pool_info.maxSets = static_cast<uint32_t>(max_image_count);
    chk(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool));

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
    v_buffer.destroy(allocator);
    i_buffer.destroy(allocator);
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
    vmaDestroyAllocator(allocator);
    vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
    for (auto& [id, layouts] : descriptor_set_layouts) {
        for (VkDescriptorSetLayout layout : layouts) {
            vkDestroyDescriptorSetLayout(device, layout, nullptr);
        }
    }
    vkDestroyDevice(device, nullptr);
    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
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

void Renderer::upload_pipeline(ShaderID vertex_shader_id, ShaderID fragment_shader_id) {
    const PipelineID pipeline_id(vertex_shader_id, fragment_shader_id);
    if (!pipelines.contains(pipeline_id)) {
        const ShaderData& vert_shader_data = shaders[vertex_shader_id];
        const ShaderData& frag_shader_data = shaders[fragment_shader_id];
    
        // Descriptor set layout
        BindingsMap bindings_map;
        const uint32_t num_sets = std::max(vert_shader_data.max_descriptor_set(), frag_shader_data.max_descriptor_set()) + 1;
        bindings_map.resize(num_sets);
        vert_shader_data.append_layout_bindings(bindings_map);
        frag_shader_data.append_layout_bindings(bindings_map);
    
        std::vector<VkDescriptorSetLayout>& layouts = descriptor_set_layouts[pipeline_id];
        layouts.resize(bindings_map.size());
        for (uint64_t i = 0; i < bindings_map.size(); ++i) {
            VkDescriptorSetLayoutCreateInfo layout_info = initializers::descriptor_set_create_info(bindings_map[i]);
            chk(vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &layouts[i]));
        }
    
        // Push constants
        std::vector<VkPushConstantRange> push_constant_ranges;
        vert_shader_data.append_push_constant_ranges(push_constant_ranges);
        frag_shader_data.append_push_constant_ranges(push_constant_ranges);
    
        // Pipeline layout
        VkPipelineLayout& pipeline_layout = pipeline_layouts[pipeline_id];
        VkPipelineLayoutCreateInfo pipeline_layout_ci = initializers::pipeline_layout_create_info(layouts);
        pipeline_layout_ci.pPushConstantRanges = push_constant_ranges.data();
        pipeline_layout_ci.pushConstantRangeCount = push_constant_ranges.size();
        chk(vkCreatePipelineLayout(device, &pipeline_layout_ci, nullptr, &pipeline_layout));
    
        // Pipeline
        pipelines.try_emplace(pipeline_id,
            this,
            pipeline_id,
            sample_count,
            image_format);
    }
}

void Renderer::upload_material(TextureID texture_id, ShaderID vertex_shader_id, ShaderID fragment_shader_id, uint32_t set_idx) {
    PipelineID pipeline_id(vertex_shader_id, fragment_shader_id);
    MaterialID material_id(texture_id, vertex_shader_id, fragment_shader_id);
    if (!materials.contains(material_id)) {
        const std::vector<VkDescriptorSetLayout>& layouts = descriptor_set_layouts[pipeline_id];
        const Texture& texture = textures.at(texture_id);
        materials.try_emplace(material_id, this, &texture, layouts[set_idx]);
    }
}

void Renderer::upload_index_data(void* data, uint64_t size_bytes) {
    i_buffer = Buffer(
        allocator,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        data,
        size_bytes
    );
}

void Renderer::upload_vertex_data(void* data, uint64_t size_bytes) {
    v_buffer = Buffer(
        allocator,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        data,
        size_bytes
    );
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

void Renderer::render(Window& window, std::vector<DrawCommand> draws) {
    vkWaitForFences(device, 1, &fences[frame_index], true, UINT64_MAX);
    vkResetFences(device, 1, &fences[frame_index]);

    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, present_semaphores[frame_index], VK_NULL_HANDLE, &image_index);

    VkCommandBuffer& cb = command_buffers[frame_index];
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

    VkRenderingAttachmentInfo colorAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = render_image_view,
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
        .resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
        .resolveImageView = swapchain_image_views[image_index],
        .resolveImageLayout = VK_IMAGE_LAYOUT_GENERAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {.color = { 0.0f, 0.0f, 0.2f, 1.0f }}
    };
    Size2Di32 window_size = window.get_size();
    VkRenderingInfo renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .extent = {
                .width = (uint32_t) window_size.width,
                .height = (uint32_t) window_size.height
            }
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
    };
    vkCmdBeginRendering(cb, &renderingInfo);

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
        .extent = {
            .width = (uint32_t) window_size.width,
            .height = (uint32_t) window_size.height
        }
    };
    vkCmdSetScissor(cb, 0, 1, &scissor);

    VkDeviceSize v_offset = 0;
    vkCmdBindVertexBuffers(cb, 0, 1, &v_buffer.raw, &v_offset);
    vkCmdBindIndexBuffer(cb, i_buffer.raw, 0, VK_INDEX_TYPE_UINT32);

    for (const DrawCommand& draw : draws) {
        const PipelineID pipeline_id(draw.vertex_id, draw.fragment_id); 
        const MaterialID material_id(draw.texture_id, draw.vertex_id, draw.fragment_id);
        const Material& material = materials[material_id];
        const Pipeline& pipeline = pipelines[pipeline_id];
        const VkPipelineLayout& pipeline_layout = pipeline_layouts[pipeline_id];

        vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.raw);
        vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &material.descriptor_set, 0, nullptr);

        for (const PushConstantData& pc : draw.pcs) {
            vkCmdPushConstants(cb, pipeline_layout, pc.stage_flags, pc.offset, pc.size, pc.p_data);
        }

        vkCmdDrawIndexed(cb, draw.index_count, 1, draw.first_index, 0, 0);
    }

    vkCmdEndRendering(cb);

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
        .pWaitSemaphores = &present_semaphores[frame_index],
        .pWaitDstStageMask = &wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &cb,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &render_semaphores[frame_index],
    };
    vkQueueSubmit(graphics_queue, 1, &submit_info, fences[frame_index]);
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &render_semaphores[frame_index],
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_index
    };
    chk(vkQueuePresentKHR(graphics_queue, &present_info));

    ++frame_index;
    if (frame_index >= MAX_FRAMES_IN_FLIGHT) {
        frame_index = 0;
    }
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