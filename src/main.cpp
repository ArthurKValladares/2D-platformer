#include <SDL3/SDL_main.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <filesystem>

#include "renderer/renderer.h"

#include "assets.h"
#include "window.h"
#include "util.h"
#include "quad_draw.h"
#include "view.h"
#include "image.h"

// TODO: The way I'm handling descriptor sets is very messy.
// Look at using push descriptors instead
// https://github.com/SaschaWillems/Vulkan/blob/master/examples/pushdescriptors/pushdescriptors.cpp

int main(int argc, char *argv[]) {
    Window window = Window();

    Renderer::enabled_features13.dynamicRendering = true;
    Renderer renderer(window);
    
    // Textures
    ImageData test_1 = ImageData(texture_path(TextureSource::Test1));
    renderer.upload_texture(static_cast<uint32_t>(TextureSource::Test1), test_1.texture_create_info());

    ImageData test_2 = ImageData(texture_path(TextureSource::Test2));
    renderer.upload_texture(static_cast<uint32_t>(TextureSource::Test2), test_2.texture_create_info());

    ImageData test_3 = ImageData(texture_path(TextureSource::Test3));
    renderer.upload_texture(static_cast<uint32_t>(TextureSource::Test3), test_3.texture_create_info());

    ImageData test_4 = ImageData(texture_path(TextureSource::Test4));
    renderer.upload_texture(static_cast<uint32_t>(TextureSource::Test4), test_4.texture_create_info());

    // Shaders
    renderer.upload_shader(static_cast<uint32_t>(ShaderSource::TriangleVert), shader_path(ShaderSource::TriangleVert));
    renderer.upload_shader(static_cast<uint32_t>(ShaderSource::TriangleTransformVert), shader_path(ShaderSource::TriangleTransformVert));
    renderer.upload_shader(static_cast<uint32_t>(ShaderSource::TriangleFrag), shader_path(ShaderSource::TriangleFrag));

    // Pipelines
    renderer.upload_pipeline(static_cast<uint32_t>(ShaderSource::TriangleVert), static_cast<uint32_t>(ShaderSource::TriangleFrag));
    renderer.upload_pipeline(static_cast<uint32_t>(ShaderSource::TriangleTransformVert), static_cast<uint32_t>(ShaderSource::TriangleFrag));

    // TODO: I should be creating these on the fly as needed and not here, do that later
    // Materials
    renderer.upload_material(static_cast<uint32_t>(TextureSource::Test1), static_cast<uint32_t>(ShaderSource::TriangleVert), static_cast<uint32_t>(ShaderSource::TriangleFrag));
    renderer.upload_material(static_cast<uint32_t>(TextureSource::Test2), static_cast<uint32_t>(ShaderSource::TriangleTransformVert), static_cast<uint32_t>(ShaderSource::TriangleFrag));
    renderer.upload_material(static_cast<uint32_t>(TextureSource::Test3), static_cast<uint32_t>(ShaderSource::TriangleVert), static_cast<uint32_t>(ShaderSource::TriangleFrag));
    renderer.upload_material(static_cast<uint32_t>(TextureSource::Test4), static_cast<uint32_t>(ShaderSource::TriangleVert), static_cast<uint32_t>(ShaderSource::TriangleFrag));

    // View-tree
    // TODO: Maybe should use the material instead of all the separete things?
    View root_view = View();

    ShaderPair pair = ShaderPair();
    pair.with_triangle_vert(TriangleVert());
    pair.with_triangle_frag(TriangleFrag(TextureSource::Test1));
    root_view.push_child(QuadDraw{
        Rect2D(Point2Df32{ -0.5f,  0.5f }, Size2Df32{1.0, 1.0}),
        pair
    });

    pair.with_triangle_transform_vert(
        TriangleTransformVert(TriangleTransformPC{
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0))
        })
    );
    pair.with_triangle_frag(TriangleFrag(TextureSource::Test2));
    root_view.push_child(View(
        QuadDraw{
            Rect2D(Point2Df32{  0.5f,  0.5f }, Size2Df32{1.0, 1.0}),
            pair
        }
    ));

    pair.with_triangle_vert(TriangleVert());
    pair.with_triangle_frag(TriangleFrag(TextureSource::Test3));
    root_view.push_child(QuadDraw{
        Rect2D(Point2Df32{ -0.5f, -0.5f }, Size2Df32{1.0, 1.0}),
        pair
    });

    pair.with_triangle_vert(TriangleVert());
    pair.with_triangle_frag(TriangleFrag(TextureSource::Test4));
    root_view.push_child(QuadDraw{
        Rect2D(Point2Df32{  0.5f, -0.5f }, Size2Df32{1.0, 1.0}),
        pair
    });

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