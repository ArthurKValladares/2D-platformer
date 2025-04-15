#include <SDL3/SDL_main.h>
#include "imgui.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <math.h> 
#include <algorithm>

#include "renderer/renderer.h"

#include "assets.h"
#include "window.h"
#include "util.h"
#include "image.h"
#include "keyboard_state.h"
#include "camera.h"
#include "animatable.h"
#include "particles.h"

#include "renderables/includes.h"

// TODO: engine architecture
// descriptor set number 0 will be used for engine-global resources
// descriptor set number 1 will be used for per-object resources (using push descriptors)

struct GlobalDescriptorSetData {
    GlobalDescriptorSetData(Renderer& renderer, const OrthographicCamera& camera) 
        : layout_id(renderer.upload_descriptor_set_layout(get_global_set_bindings()))
        , set_id(renderer.upload_descriptor_set(layout_id))
        , shader_data(GlobalShaderData{
            .proj_matrix = camera.get_proj_matrix()
        })
        , buffer_id(renderer.request_buffer(
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU,
            sizeof(GlobalShaderData)
        ))
    {}

    void write_shader_data_to_buffer(Renderer& renderer) {
        Buffer& buffer = renderer.get_buffer(buffer_id);
        buffer.write_to(&shader_data, sizeof(GlobalShaderData));
    }

    DescriptorSetLayoutID layout_id;
    DescriptorSetID set_id;
    GlobalShaderData shader_data;
    BufferID buffer_id;
};

struct Level1 {
    Rect2D regular_quad;
    Rect2D moving_quad;

    Rect2D colored_quad;
    glm::vec3 quad_color;

    Rect2D data_quad;
    glm::vec2 data_pos_offset;
    glm::vec3 data_color;

    Level1()
        : regular_quad(Rect2D(glm::vec2(-0.5f, 0.5f), glm::vec2(1.0, 1.0)))
        , moving_quad(Rect2D(glm::vec2(0.5f, 0.5f), glm::vec2(1.0, 1.0)))
        , colored_quad(Rect2D(glm::vec2(-0.5f, -0.5f), glm::vec2(1.0, 1.0)))
        , data_quad(Rect2D(glm::vec2(0.5f, -0.5f), glm::vec2(1.0, 1.0)))
        , data_pos_offset(glm::vec2(0.0f, 0.0f))
    {}

    Renderable build(Renderer& renderer, BufferID global_data_buffer, double total_elapsed_seconds) {
        const double x_offset = sin(total_elapsed_seconds) * 0.1;
        const double y_offset = cos(total_elapsed_seconds) * 0.1;

        quad_color.r = abs(sin(total_elapsed_seconds));
        quad_color.g = abs(cos(total_elapsed_seconds * 0.5));
        quad_color.b = abs(tan(total_elapsed_seconds * 0.25));

        data_color.r = abs(tan(total_elapsed_seconds));
        data_color.g = abs(sin(total_elapsed_seconds * 0.5));
        data_color.b = abs(cos(total_elapsed_seconds * 0.25));
        data_pos_offset.y = y_offset;

        Renderable renderable;
        renderable.push_child(Quad(
            &renderer,
            regular_quad,
            TextureSource::Test1,
            global_data_buffer
        ));
        renderable.push_child(MovingQuad(
            &renderer,
            moving_quad,
            glm::vec2(x_offset, 0.0),
            TextureSource::Test2,
            global_data_buffer
        ));
        renderable.push_child(ColoredQuad(
            &renderer,
            colored_quad,
            TextureSource::Test3,
            quad_color,
            global_data_buffer
        ));
        renderable.push_child(DataQuad(
            &renderer,
            data_quad,
            TextureSource::Test4,
            global_data_buffer,
            data_pos_offset,
            data_color
        ));
        return renderable;
    }
};

struct Level2 {
    Rect2D colored_quad;
    glm::vec3 quad_color;

    Level2()
        : colored_quad(Rect2D(glm::vec2(0.0f, 0.0f), glm::vec2(2.0, 2.0)))
    {}

    Renderable build(Renderer& renderer, BufferID global_data_buffer, double total_elapsed_seconds) {
        quad_color.r = abs(sin(total_elapsed_seconds));
        quad_color.g = abs(cos(total_elapsed_seconds * 0.5));
        quad_color.b = abs(tan(total_elapsed_seconds * 0.25));

        Renderable renderable;
        renderable.push_child(ColoredQuad(
            &renderer,
            colored_quad,
            TextureSource::Akv,
            quad_color,
            global_data_buffer
        ));
        return renderable;
    }
};

struct App {
    App() 
        : app_start(std::chrono::steady_clock::now())
        , keyboard_state(KeyboardState())
        , camera(OrthographicCamera(glm::vec2(0.0), camera_scale, camera_scale))
        , window(Window())
        , renderer(window)
        , global_set_data(GlobalDescriptorSetData(renderer, camera))
        , player_rect(Rect2D(glm::vec2(0.0f, 0.0f), glm::vec2(0.25, 0.25)))
        , player_sprite(3.0, 0.0, {TextureSource::Test1, TextureSource::Test2, TextureSource::Test3, TextureSource::Test4})
        , emitter(ParticleEmitter(
            0.0,
            glm::vec2(0.0),
            0.00,
            Degrees(90.0),
            2.0,
            1.0,
            glm::vec2(0.3, 0.3),
            glm::vec3(1.0, 0.0, 0.0),
            glm::vec3(0.0, 0.0, 1.0),
            TextureSource::Particle,
            0.0,
            Degrees(15.0),
            0.5,
            0.25,
            glm::vec2(.125),
            glm::vec3(0.05),
            glm::vec3(0.05)
        ))
    {
        global_set_data.write_shader_data_to_buffer(renderer);
        update_global_set(&renderer, global_set_data.buffer_id, global_set_data.set_id);

        renderer.set_imgui_fn([](const ImguiData& data) {
            ImGui::Begin("Imgui Test");

            const uint32_t fps = 1.0 / data.frame_dt;
            ImGui::Text("Frame dt %.3f ms (%u FPS)", data.frame_dt * 1000, fps);        
        });
    }

    Renderable build_root_renderable(KeyboardState& keyboard_state, double total_elapsed_seconds, double frame_dt) {
        Renderable renderable;
        /*
        if (level_idx == 0) {
            renderable = level_1.build(renderer, global_set_data.buffer_id, total_elapsed_seconds);
        } else if (level_idx == 1) {
            renderable = level_2.build(renderer, global_set_data.buffer_id, total_elapsed_seconds);
        }
        */

        // Update player movement
        constexpr float displacement_per_second = 0.5;
        glm::vec2 movement_vec{0.0, 0.0};
        if (keyboard_state.is_down(SDLK_A)) {
            movement_vec.x -= 1.0;
        }
        if (keyboard_state.is_down(SDLK_W)){
            movement_vec.y += 1.0;
        }
        if (keyboard_state.is_down(SDLK_S)){
            movement_vec.y -= 1.0;
        }
        if (keyboard_state.is_down(SDLK_D)){
            movement_vec.x += 1.0;
        }
        if (glm::length(movement_vec) > 0.0) {
            movement_vec = glm::normalize(movement_vec);

            const float displacement = displacement_per_second * frame_dt;
            const glm::vec2 displacement_vec = movement_vec * displacement;
            player_rect.pos += displacement_vec;
        }

        //emitter.update_and_create_renderables(renderable, total_elapsed_seconds, &renderer, global_set_data.buffer_id);
        for (size_t i = 0; i < renderer.get_frame_count() % 3; ++i) {
            renderable.push_child(Quad(
                &renderer,
                Rect2D(glm::vec2(-0.5f, 0.5f), glm::vec2(1.0, 1.0)),
                TextureSource::Particle,
                global_set_data.buffer_id
            ));
        }

        renderable.push_child(MovingQuad(
            &renderer,
            player_rect,
            glm::vec2(0.0, 0.0),
            player_sprite.texture_at(total_elapsed_seconds),
            global_set_data.buffer_id
        ));
       
        return renderable;
    }

    void render(double total_elapse_seconds, double frame_dt) {
        camera.update(CameraUpdateData{
            .frame_dt = frame_dt,
            .keyboard_state = keyboard_state,
        });

        global_set_data.shader_data.proj_matrix = camera.get_proj_matrix();
        global_set_data.write_shader_data_to_buffer(renderer);

        Renderable curr_renderable = build_root_renderable(keyboard_state, total_elapse_seconds, frame_dt);
        ViewDrawData data = curr_renderable.get_draw_data(&renderer);
        data.upload_vertex_index_data(&renderer);
        
        renderer.setup_imgui_draw(ImguiData{
            .frame_dt = frame_dt
        });

        renderer.render(window, data.draws);
    }

    void render_loop() {
        last_frame = std::chrono::steady_clock::now();

        SDL_Event e;
        SDL_zero(e);

        bool quit = false;
        while (!quit) {
            const std::chrono::steady_clock::time_point frame_start = std::chrono::steady_clock::now();
            const std::chrono::duration<double>         elapsed_seconds = frame_start - app_start;
            const std::chrono::duration<double>         frame_dt = frame_start - last_frame;
            last_frame = frame_start;
    
            keyboard_state.reset();
            while(SDL_PollEvent(&e)) {
                renderer.process_sdl_event(&e);
                keyboard_state.process_sdl_event(e.key);

                if (e.type == SDL_EVENT_QUIT ) {
                    quit = true;
                } else if (e.type = SDL_EVENT_WINDOW_RESIZED) {
                    renderer.resize_swapchain(window);
                }
            }
            if (keyboard_state.is_down(SDLK_ESCAPE)) {
                quit = true;
            }
            if (keyboard_state.was_just_released(SDLK_P)) {
                level_idx = (level_idx + 1) % num_levels;
            }

            constexpr float camera_zoom_vel = 0.5;
            if (keyboard_state.is_down(SDLK_E)) {
                camera_scale += camera_zoom_vel * frame_dt.count();
            }
            if (keyboard_state.is_down(SDLK_Q)) {
                camera_scale -= camera_zoom_vel * frame_dt.count();
                camera_scale = std::max(0.1f, camera_scale);
            }
            const float camera_size = exp(camera_scale) * 0.125;
            camera.size_x = camera_size;
            camera.size_y = camera_size;

            render(elapsed_seconds.count(), frame_dt.count());
        }
    }

    std::chrono::steady_clock::time_point app_start;
    KeyboardState keyboard_state;
    float camera_scale = 2.77;
    OrthographicCamera camera;
    Window window;
    Renderer renderer;
    GlobalDescriptorSetData global_set_data;
    std::chrono::steady_clock::time_point last_frame;

    // Level stuff, bad and temp
    Rect2D player_rect;
    SpriteAnimation player_sprite;

    ParticleEmitter emitter;

    Level1 level_1;
    Level2 level_2;

    uint32_t num_levels = 2;
    uint32_t level_idx = 0;
};

int main(int argc, char *argv[]) {
    App app = App();
    
    app.render_loop();

    return 0;
}