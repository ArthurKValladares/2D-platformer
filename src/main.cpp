#include <SDL3/SDL_main.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <math.h> 

#include "renderer/renderer.h"

#include "assets.h"
#include "window.h"
#include "util.h"
#include "image.h"
#include "keyboard_state.h"
#include "camera.h"

#include "views/view.h"

// TODO: engine architecture
// descriptor set number 0 will be used for engine-global resources
// descriptor set number 1 will be used for per-object resources (using push descriptors)

int main(int argc, char *argv[]) {
    const std::chrono::steady_clock::time_point app_start = std::chrono::steady_clock::now();

    Window window = Window();
    OrthographicCamera camera = OrthographicCamera(glm::vec2(0.0), 2.0, 2.0);

    Renderer renderer(window);
    
    // TODO: Doing this here for now, very sloppy tho.
    std::vector<VkDescriptorSetLayoutBinding> bindings = {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
        }
    };
    const DescriptorSetLayoutID layout_id = renderer.upload_descriptor_set_layout(bindings);
    const DescriptorSetID set_id = renderer.upload_descriptor_set(layout_id);
   
    // Create global data buffer
    GlobalData global_data = GlobalData{
        .proj_matrix = camera.get_proj_matrix()
    };
    BufferID global_data_buffer = renderer.request_buffer(
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VMA_ALLOCATION_CREATE_MAPPED_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU,
        sizeof(GlobalData)
    );
    Buffer& buffer = renderer.get_buffer(global_data_buffer);
    buffer.write_to(&global_data, sizeof(GlobalData));

    // Write buffer to global set
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = buffer.raw;
    buffer_info.offset = 0;
    buffer_info.range = sizeof(GlobalData);
    std::array<VkWriteDescriptorSet, 1> descriptor_writes{};
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = renderer.get_descriptor_set_at(set_id);
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;
    vkUpdateDescriptorSets(renderer.get_device(), static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);

    // View-tree
    View root_view = View();
    root_view.push_child(QuadDraw(
        &renderer,
        Rect2D(Point2Df32{ -0.5f,  0.5f }, Size2Df32{1.0, 1.0}),
        TextureSource::Test1,
        global_data_buffer
    ));
    root_view.push_child(MovingQuadDraw(
        &renderer,
        Rect2D(Point2Df32{  0.5f,  0.5f }, Size2Df32{1.0, 1.0}),
        TextureSource::Test2,
        global_data_buffer
    )
    );
    root_view.push_child(ColorQuadDraw(
        &renderer,
        Rect2D(Point2Df32{ -0.5f, -0.5f }, Size2Df32{1.0, 1.0}),
        TextureSource::Test3,
        global_data_buffer
    ));
    root_view.push_child(DataQuadDraw(
        &renderer,
        Rect2D(Point2Df32{  0.5f, -0.5f }, Size2Df32{1.0, 1.0}),
        TextureSource::Test4,
        global_data_buffer
    ));
    root_view.push_child(ControllableQuadDraw(
        &renderer,
        Rect2D(Point2Df32{ 0.0f,  0.0f }, Size2Df32{0.5, 0.5}),
        0.0,
        {TextureSource::Akv, TextureSource::Test1, TextureSource::Test2, TextureSource::Test3, TextureSource::Test4},
        global_data_buffer
    ));

    KeyboardState keyboard_state;
    std::chrono::steady_clock::time_point last_frame = std::chrono::steady_clock::now();
    SDL_Event e;
    SDL_zero(e);
    bool quit = false;
    while (!quit) {
        const std::chrono::steady_clock::time_point frame_start = std::chrono::steady_clock::now();
        const std::chrono::duration<double>         elapsed_seconds = frame_start - app_start;
        const std::chrono::duration<double>         frame_dt = frame_start - last_frame;
        last_frame = frame_start;

        while(SDL_PollEvent(&e)) {
            renderer.process_sdl_event(&e);
            if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
                keyboard_state.on_keyboard_event(e.key);
            } else if (e.type == SDL_EVENT_QUIT ) {
                quit = true;
            } else if (e.type = SDL_EVENT_WINDOW_RESIZED) {
                renderer.resize_swapchain(window);
            }
        }
        if (keyboard_state.is_down(SDLK_ESCAPE)) {
            quit = true;
        }

        camera.update(CameraUpdateData{
            .frame_dt = frame_dt.count(),
            .keyboard_state = keyboard_state,
        });
        global_data.proj_matrix = camera.get_proj_matrix();
        Buffer& buffer = renderer.get_buffer(global_data_buffer);
        buffer.write_to(&global_data, sizeof(GlobalData));

        root_view.update(ViewUpdateData{
            .renderer = &renderer,
            .total_elapsed_seconds = elapsed_seconds.count(),
            .frame_dt = frame_dt.count(),
            .keyboard_state = keyboard_state,
        });
        ViewDrawData data = root_view.get_draw_data(&renderer);
        data.upload_vertex_index_data(&renderer);
        
        renderer.setup_imgui_draw(ImguiData{
            .frame_dt = frame_dt.count()
        });

        renderer.render(window, data.draws);
    }

    return 0;
}