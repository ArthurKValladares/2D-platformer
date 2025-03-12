#include <SDL3/SDL_main.h>

#include <iostream>
#include <fstream>
#include <filesystem>

#include "renderer/renderer.h"
#include "asset_manager.h"

#include "window.h"
#include "util.h"
#include "quad_draw.h"
#include "view.h"

int main(int argc, char *argv[]) {
    Window window = Window();

    Renderer::enabled_features13.dynamicRendering = true;
    Renderer renderer(window);
    
    // Textures
    TextureManager texture_manager;

    std::vector<TextureCreateInfo> texture_cis;
    texture_cis.reserve(texture_manager.images.size());
    for (ImageData& image : texture_manager.images) {
        texture_cis.push_back(TextureCreateInfo{
            .buffer = image.img,
            .buffer_size = image.size,
            .width = static_cast<uint32_t>(image.width),
            .height = static_cast<uint32_t>(image.height),
            .format = VK_FORMAT_R8G8B8A8_SRGB,
        });
    }
    renderer.upload_textures(texture_cis);

    // View-tree
    View root_view = View();
    root_view.push_child(QuadDraw{Rect2D(Point2Df32{ -0.5f,  0.5f }, Size2Df32{1.0, 1.0}), TextureSource::Test1});
    root_view.push_child(QuadDraw{Rect2D(Point2Df32{  0.5f,  0.5f }, Size2Df32{1.0, 1.0}), TextureSource::Test2});
    root_view.push_child(QuadDraw{Rect2D(Point2Df32{ -0.5f, -0.5f }, Size2Df32{1.0, 1.0}), TextureSource::Test3});
    root_view.push_child(QuadDraw{Rect2D(Point2Df32{  0.5f, -0.5f }, Size2Df32{1.0, 1.0}), TextureSource::Test4});

    ViewDrawData data = {};
    root_view.append_draw_data(&renderer, data);
    renderer.upload_index_data(&data.indices[0], data.indices.size() * sizeof(uint32_t));
    renderer.upload_vertex_data(&data.vertices[0], data.vertices.size() * sizeof(QuadVertex));

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
            } else if (e.type = SDL_EVENT_WINDOW_RESIZED) {
                renderer.resize_swapchain(window);
            }
        }

        renderer.render(window, data.draws);
    }

    return 0;
}