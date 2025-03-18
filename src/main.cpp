#include <SDL3/SDL_main.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <math.h> 

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
    
    // View-tree
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
            glm::mat4(1.0)
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

    ViewDrawData data = root_view.get_draw_data(&renderer);

    const auto start{std::chrono::steady_clock::now()};
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

        const auto finish{std::chrono::steady_clock::now()};
        const std::chrono::duration<double> elapsed_seconds{finish - start};
        const double offset = sin(elapsed_seconds.count()) * 0.1;

        // TODO: Obviously bad, need like an `update` function on views or something like that
        root_view.children[1].draw.shaders.triangle_transform_vert.push_constant.render_matrix = 
        glm::translate(glm::mat4(1.0f), glm::vec3(offset, 0.0, 0.0));
        
        renderer.render(window, data.draws);
    }

    return 0;
}