#define VOLK_IMPLEMENTATION
#include "volk.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif

namespace {
    inline void chk(VkResult result) {
        if (result != VK_SUCCESS) {
            std::cerr << "Vulkan call returned an error\n";
            exit(result);
        }
    }
    inline void chk(bool result) {
        if (!result) {
            std::cerr << "Call returned an error\n";
            exit(result);
        }
    }
    inline void chk(HRESULT result) {
        if (FAILED(result)) {
            std::cerr << "Call returned an error\n";
            exit(result);
        }
    }

    inline void chk_sdl(bool result) {
        if (!result) {
            std::cerr << SDL_GetError() << std::endl;
            exit(result);
        }
    }

    template<class T>
    std::vector<T> read_file_to_buffer(const std::string& path) {
        if (!std::filesystem::exists(path)) {
            std::cerr << "file: " << path << " does not exist" << std::endl;
            exit(-1);
        }

        std::ifstream source_file(path, std::ios::ate | std::ios::binary);
        if (!source_file.is_open()) {
            std::cerr << "Could not open file: " << path << std::endl;
            exit(-1);
        }

        const size_t file_size_bytes = (size_t) source_file.tellg();
        std::vector<uint32_t> buffer(file_size_bytes / sizeof(T));

        source_file.seekg(0);
        source_file.read((char*)buffer.data(), file_size_bytes);
        source_file.close();
        
        return buffer;
    }

    bool check_validation_layer_support(const std::vector<const char*>& validation_layers) {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        for (const char* layerName : validation_layers) {
            bool layer_found = false;

            for (const auto& layerProperties : available_layers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found) {
                return false;
            }
        }

        return true;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

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

int main(int argc, char *argv[]) {
    //
    // Init SDL
    //
    constexpr int SCREEN_WIDTH = 1200;
    constexpr int SCREEN_HEIGHT = 800;

    SDL_Window* window = nullptr;
    SDL_Surface* screen_surface = nullptr;

    chk_sdl(SDL_Init(SDL_INIT_VIDEO));

    const int window_flags = SDL_WINDOW_VULKAN;
    window = SDL_CreateWindow("2D-Platformer", SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
    if(window == NULL) {
        SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
        return -1;
    }
    screen_surface = SDL_GetWindowSurface(window);

    //
    // Init Vulkan
    //
    constexpr uint32_t max_frames_in_flight = 2;
    const VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_4_BIT;
    uint32_t image_index = 0;
    uint32_t frame_index = 0;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;

    VmaAllocator allocator = VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;

    VkImage render_image;
    VmaAllocation render_image_allocation;
    VkImageView render_image_view;

    VmaAllocation v_buffer_allocation = VK_NULL_HANDLE;
    VkBuffer v_buffer = VK_NULL_HANDLE;

    VkCommandPool command_pool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> command_buffers(max_frames_in_flight);
    std::vector<VkFence> fences(max_frames_in_flight);
    std::vector<VkSemaphore> present_semaphores(max_frames_in_flight);
    std::vector<VkSemaphore> render_semaphores(max_frames_in_flight);

    VkPipeline pipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;

    // External libs
    volkInitialize();

    // Instance
	VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "2D-Platformer",
        .apiVersion = VK_API_VERSION_1_3
    };
	const std::vector<const char*> instance_extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
	VkInstanceCreateInfo instance_ci = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
		.enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size()),
		.ppEnabledExtensionNames = instance_extensions.data(),
	};
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    if (enable_validation_layers && !check_validation_layer_support(validation_layers)) {
        std::cerr << "validation layers requested, but not available!" << std::endl;
        exit(-1);
    }
    if (enable_validation_layers) {
        instance_ci.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        instance_ci.ppEnabledLayerNames = validation_layers.data();
    }
	chk(vkCreateInstance(&instance_ci, nullptr, &instance));
	volkLoadInstance(instance);

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_ci = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback
    };
    vkCreateDebugUtilsMessengerEXT(instance, &debug_messenger_ci, nullptr, &debug_messenger);

    // Device
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());
    const uint32_t q_family_idx = 0;
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
    chk_sdl(SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface));
    int window_width;
    int window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    const VkFormat image_format = VK_FORMAT_B8G8R8A8_SRGB;
    const VkColorSpaceKHR color_space = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    VkSwapchainCreateInfoKHR swapchainCI = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = 2,
        .imageFormat = image_format,
        .imageColorSpace = color_space,
        .imageExtent = {.width = (uint32_t) window_width, .height = (uint32_t) window_height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .queueFamilyIndexCount = q_family_idx,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR

    };
    chk(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapchain));
    uint32_t image_count{ 0 };
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
    swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());
    swapchain_image_views.resize(image_count);
    VkImageCreateInfo renderImageCI = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = image_format,
        .extent = {.width = (uint32_t) window_width, .height = (uint32_t) window_height, .depth = 1 },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = sample_count,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    VmaAllocationCreateInfo allocCI = {
        .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.0f
    };
    vmaCreateImage(allocator, &renderImageCI, &allocCI, &render_image, &render_image_allocation, nullptr);
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
        0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    VkBufferCreateInfo buffer_ci = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(float) * vertices.size(),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    };
    VmaAllocationCreateInfo buffer_alloc_ci = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO
    };
    chk(vmaCreateBuffer(allocator, &buffer_ci, &buffer_alloc_ci, &v_buffer, &v_buffer_allocation, nullptr));
    void* buffer_ptr = nullptr;
    vmaMapMemory(allocator, v_buffer_allocation, &buffer_ptr);
    memcpy(buffer_ptr, vertices.data(), sizeof(float)* vertices.size());
    vmaUnmapMemory(allocator, v_buffer_allocation);

    // Command Pool
    VkCommandPoolCreateInfo command_pool_ci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = q_family_idx
    };
    chk(vkCreateCommandPool(device, &command_pool_ci, nullptr, &command_pool));

    // Sync objects
    for (auto i = 0; i < max_frames_in_flight; i++) {
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
    
    //
    // Main loop
    //
    SDL_Event e;
    SDL_zero(e);

    bool quit = false;
    while (!quit) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT ) {
                quit = true;
            } else if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    quit = true;
                }
            }
        }
    }

    // Cleanup Vulkan
    vkDeviceWaitIdle(device);
    for (auto i = 0; i < max_frames_in_flight; i++) {
        vkDestroyFence(device, fences[i], nullptr);
        vkDestroySemaphore(device, present_semaphores[i], nullptr);
        vkDestroySemaphore(device, render_semaphores[i], nullptr);
    }
    vmaDestroyImage(allocator, render_image, render_image_allocation);
    vkDestroyImageView(device, render_image_view, nullptr);
    for (auto i = 0; i < swapchain_image_views.size(); i++) {
        vkDestroyImageView(device, swapchain_image_views[i], nullptr);
    }
    vmaDestroyBuffer(allocator, v_buffer, v_buffer_allocation);
    vkDestroyCommandPool(device, command_pool, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vmaDestroyAllocator(allocator);
    vkDestroyDevice(device, nullptr);
    vkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
    vkDestroyInstance(instance, nullptr);

    //
    // Cleanup SDL
    //
    SDL_DestroySurface(screen_surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
}