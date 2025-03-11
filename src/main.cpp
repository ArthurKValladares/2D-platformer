#include <SDL3/SDL_main.h>

#include <iostream>
#include <fstream>
#include <filesystem>

#include "renderer/renderer.h"
#include "asset_manager.h"

#include "window.h"
#include "util.h"
#include "quad_draw.h"

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

    // Quads
    // TODO: Need to attach the textures to this and get the draws from it
    std::vector<QuadVertex> vertices = {};
    std::vector<uint32_t> indices = {};
    std::vector<QuadDraw> quad_draws = {
        {Rect2D(Point2Df32{ -0.5f,  0.5f }, Size2Df32{1.0, 1.0}), TextureSource::Test1},
        {Rect2D(Point2Df32{  0.5f,  0.5f }, Size2Df32{1.0, 1.0}), TextureSource::Test2},
        {Rect2D(Point2Df32{ -0.5f, -0.5f }, Size2Df32{1.0, 1.0}), TextureSource::Test3},
        {Rect2D(Point2Df32{  0.5f, -0.5f }, Size2Df32{1.0, 1.0}), TextureSource::Test4}
    };

    std::vector<DrawCommand> draws = {};
    for (QuadDraw& quad_draw : quad_draws) {
        const uint32_t first_index = indices.size();

        const uint32_t index_count = quad_draw.rect.index_data(vertices.size(), indices);
        quad_draw.rect.vertex_data(vertices);

        // TODO: I need a better way to get the actual material I want
        const Material& material = renderer.get_material_at(static_cast<uint64_t>(quad_draw.texture));
        draws.push_back(DrawCommand{
            .descriptor_set = material.descriptor_set,
            .index_count = index_count,
            .first_index = first_index
        });
    }

    renderer.upload_index_data(&indices[0], indices.size() * sizeof(uint32_t));
    renderer.upload_vertex_data(&vertices[0], vertices.size() * sizeof(QuadVertex));
    
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

        renderer.render(window, draws);
    }

    return 0;
}