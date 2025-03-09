#include <iostream>

#define VMA_IMPLEMENTATION
#include "renderer.h"
#include "initializers.h"
#include "tools.h"

#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>

#include "../util.h"
#include "../window.h"

#ifdef NDEBUG
constexpr bool USE_VALIDATION_LAYERS = false;
#else
constexpr bool USE_VALIDATION_LAYERS = true;
#endif

namespace {
    VkPipelineShaderStageCreateInfo create_shader(VkDevice device, const std::vector<uint32_t>& shader_bytes, VkShaderStageFlagBits shaderStage) {
        VkShaderModuleCreateInfo shaderModule_ci = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = shader_bytes.size() * sizeof(uint32_t),
            .pCode = shader_bytes.data()
        };
        VkShaderModule shader_module;
        vkCreateShaderModule(device, &shaderModule_ci, nullptr, &shader_module);
        return VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = shaderStage,
            .module = shader_module,
            .pName = "main"
        };
    };
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
        .queueFamilyIndexCount = q_family_idx,
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

    // Device
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());
    const float q_family_prios = 1.0f;
    const uint32_t device_index = 0;
    VkDeviceQueueCreateInfo queue_ci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = q_family_idx,
        .queueCount = 1,
        .pQueuePriorities = &q_family_prios
    };
    VkPhysicalDeviceVulkan13Features features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .dynamicRendering = true
    };
    const std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    VkDeviceCreateInfo device_ci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_ci,
        .enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
        .ppEnabledExtensionNames = device_extensions.data(),
    };
    chk(vkCreateDevice(devices[device_index], &device_ci, nullptr, &device));
    vkGetDeviceQueue(device, q_family_idx, 0, &queue);

    // VMA
	VmaVulkanFunctions vk_functions = {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        .vkCreateImage = vkCreateImage
    };
	VmaAllocatorCreateInfo allocator_ci = {
        .physicalDevice = devices[device_index],
        .device = device,
        .pVulkanFunctions = &vk_functions,
        .instance = instance
    };
	chk(vmaCreateAllocator(&allocator_ci, &allocator));

    // Presentation
    chk_sdl(SDL_Vulkan_CreateSurface(window.raw, instance, nullptr, &surface));
    Size2D window_size = window.get_size();
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

    // Vertexbuffer (Pos 3f, Col 3f)
    const std::vector<float> vertices = {
         -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
          0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
          0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
    };
    v_buffer = Buffer(
        allocator,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        vertices
    );

    // index buffer
    const std::vector<uint32_t> indices = {
        0, 1, 2,
        0, 3, 1
    };
    num_indices = indices.size();
    i_buffer = Buffer(
        allocator,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        indices
    );

    // Texture
    ImageData test_image = ImageData("assets/textures/akv.png");
    texture = Texture(this, TextureCreateInfo{
        .buffer = test_image.img,
        .buffer_size = test_image.size,
        .width = static_cast<uint32_t>(test_image.width),
        .height = static_cast<uint32_t>(test_image.height),
        .format = VK_FORMAT_R8G8B8A8_SRGB,
    });

    // Command Pool
    VkCommandPoolCreateInfo command_pool_ci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = q_family_idx
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

    // Pipeline
	VkPipelineLayoutCreateInfo pipeline_layout_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
    };
	chk(vkCreatePipelineLayout(device, &pipeline_layout_ci, nullptr, &pipeline_layout));
    const std::vector<uint32_t> vert_shader_bytes = read_file_to_buffer<uint32_t>("shaders/triangle.vert.spv");
    const std::vector<uint32_t> frag_shader_bytes = read_file_to_buffer<uint32_t>("shaders/triangle.frag.spv");
	std::vector<VkPipelineShaderStageCreateInfo> stages = {
        create_shader(device, vert_shader_bytes, VK_SHADER_STAGE_VERTEX_BIT),
        create_shader(device, frag_shader_bytes, VK_SHADER_STAGE_FRAGMENT_BIT)
    };
	VkVertexInputBindingDescription vertex_binding = {
        .binding = 0,
        .stride = sizeof(float) * 6,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };
	std::vector<VkVertexInputAttributeDescription> vertex_attributes = {
		{.location = 0, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT},
		{.location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = sizeof(float) * 3},
	};
	VkPipelineVertexInputStateCreateInfo vertex_input_state = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertex_binding,
		.vertexAttributeDescriptionCount = 2,
		.pVertexAttributeDescriptions = vertex_attributes.data(),
	};
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };
	VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1
    };
	VkPipelineRasterizationStateCreateInfo rasterization_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .lineWidth = 1.0f
    };
	VkPipelineMultisampleStateCreateInfo multisample_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = sample_count
    };
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
    };
	VkPipelineColorBlendAttachmentState blend_attachment = {
        .colorWriteMask = 0xF
    };
	VkPipelineColorBlendStateCreateInfo color_blend_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &blend_attachment
    };
	std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
	VkPipelineDynamicStateCreateInfo dynamic_state_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamic_states.data()
    };
	VkPipelineRenderingCreateInfo rendering_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &image_format
    };
	VkGraphicsPipelineCreateInfo pipeline_ci = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &rendering_ci,
		.stageCount = 2,
		.pStages = stages.data(),
		.pVertexInputState = &vertex_input_state,
		.pInputAssemblyState = &input_assembly_state,
		.pViewportState = &viewport_state,
		.pRasterizationState = &rasterization_state,
		.pMultisampleState = &multisample_state,
		.pDepthStencilState = &depth_stencil_state,
		.pColorBlendState = &color_blend_state,
		.pDynamicState = &dynamic_state_ci,
		.layout = pipeline_layout,
	};
	chk(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_ci, nullptr, &pipeline));
	vkDestroyShaderModule(device, stages[0].module, nullptr);
	vkDestroyShaderModule(device, stages[1].module, nullptr);
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
    texture.destroy(allocator);
    vkDestroyCommandPool(device, command_pool, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vmaDestroyAllocator(allocator);
    vkDestroyDevice(device, nullptr);
    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
}

void Renderer::resize_swapchain(Window& window) {
    vkDeviceWaitIdle(device);

    Size2D window_size = window.get_size();
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

void Renderer::render(Window& window) {
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
    Size2D window_size = window.get_size();
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
        .width = static_cast<float>(window_size.width),
        .height = static_cast<float>(window_size.height),
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

    vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkDeviceSize v_offset = 0;
    vkCmdBindVertexBuffers(cb, 0, 1, &v_buffer.raw, &v_offset);
    vkCmdBindIndexBuffer(cb, i_buffer.raw, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(cb, num_indices, 1, 0, 0, 0);

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
    vkQueueSubmit(queue, 1, &submit_info, fences[frame_index]);
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &render_semaphores[frame_index],
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_index
    };
    chk(vkQueuePresentKHR(queue, &present_info));

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