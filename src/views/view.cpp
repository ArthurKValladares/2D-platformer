#include "view.h"
#include "../image.h"

#include "../renderer/renderer.h"

void View::append_draw_data(Renderer* renderer, ViewDrawData& data) const {
    // TODO: For now im just duplicating code, will be better later.
    switch (ty)
    {
        case ViewType::Quad: {
            //////
            if (!quad_draw.is_empty()) {
                const TextureSource draw_texture = quad_draw.shaders.draw_texture();
                const uint32_t first_index = data.indices.size();
        
                const uint32_t index_count = quad_draw.rect.index_data(data.vertices.size() / quad_draw.shaders.vertex_num_floats(), data.indices);
                quad_draw.rect.vertex_data(data.vertices);
        
                DrawCommand dc = DrawCommand{
                    .texture_id = static_cast<uint64_t>(draw_texture),
                    .pipeline_id = std::make_pair(
                        static_cast<uint32_t>(quad_draw.shaders.vertex_ty),
                        static_cast<uint32_t>(quad_draw.shaders.fragment_ty)
                    ),
                    .index_count = index_count,
                    .first_index = first_index
                };
                quad_draw.shaders.append_push_constant_data(dc.pcs);
                data.draws.push_back(dc);
        
                // Upload draw data
                const ImageData image_data = ImageData(texture_path(draw_texture));
                renderer->upload_texture(static_cast<uint32_t>(draw_texture), image_data.texture_create_info());
        
                renderer->upload_shader(static_cast<uint32_t>(quad_draw.shaders.vertex_ty), shader_path(quad_draw.shaders.vertex_ty));
                renderer->upload_shader(static_cast<uint32_t>(quad_draw.shaders.fragment_ty), shader_path(quad_draw.shaders.fragment_ty));
        
                renderer->upload_pipeline(static_cast<uint32_t>(quad_draw.shaders.vertex_ty), static_cast<uint32_t>(quad_draw.shaders.fragment_ty));
        
                renderer->upload_material(
                    static_cast<uint32_t>(draw_texture),
                    static_cast<uint32_t>(quad_draw.shaders.vertex_ty),
                    static_cast<uint32_t>(quad_draw.shaders.fragment_ty),
                    quad_draw.shaders.draw_texture_binding().set
                );
            }
            //////
            break;
        }
        case ViewType::MovingQuad: {
            //////
            if (!moving_quad_draw.is_empty()) {
                const TextureSource draw_texture = moving_quad_draw.shaders.draw_texture();
                const uint32_t first_index = data.indices.size();
        
                const uint32_t index_count = moving_quad_draw.rect.index_data(data.vertices.size() / moving_quad_draw.shaders.vertex_num_floats(), data.indices);
                moving_quad_draw.rect.vertex_data(data.vertices);
        
                DrawCommand dc = DrawCommand{
                    .texture_id = static_cast<uint64_t>(draw_texture),
                    .pipeline_id = std::make_pair(
                        static_cast<uint32_t>(moving_quad_draw.shaders.vertex_ty),
                        static_cast<uint32_t>(moving_quad_draw.shaders.fragment_ty)
                    ),
                    .index_count = index_count,
                    .first_index = first_index
                };
                moving_quad_draw.shaders.append_push_constant_data(dc.pcs);
                data.draws.push_back(dc);
        
                // Upload draw data
                const ImageData image_data = ImageData(texture_path(draw_texture));
                renderer->upload_texture(static_cast<uint32_t>(draw_texture), image_data.texture_create_info());
        
                renderer->upload_shader(static_cast<uint32_t>(moving_quad_draw.shaders.vertex_ty), shader_path(moving_quad_draw.shaders.vertex_ty));
                renderer->upload_shader(static_cast<uint32_t>(moving_quad_draw.shaders.fragment_ty), shader_path(moving_quad_draw.shaders.fragment_ty));
        
                renderer->upload_pipeline(static_cast<uint32_t>(moving_quad_draw.shaders.vertex_ty), static_cast<uint32_t>(moving_quad_draw.shaders.fragment_ty));
        
                renderer->upload_material(
                    static_cast<uint32_t>(draw_texture),
                    static_cast<uint32_t>(moving_quad_draw.shaders.vertex_ty),
                    static_cast<uint32_t>(moving_quad_draw.shaders.fragment_ty),
                    moving_quad_draw.shaders.draw_texture_binding().set
                );
            }
            //////
            break;
        }
    }

    for (const View& child : children) {
        child.append_draw_data(renderer, data);
    }
}

ViewDrawData View::get_draw_data(Renderer* renderer) {
    ViewDrawData data = {};

    append_draw_data(renderer, data);
    renderer->upload_index_data(&data.indices[0], data.indices.size() * sizeof(uint32_t));
    renderer->upload_vertex_data(&data.vertices[0], data.vertices.size() * sizeof(float));

    return data;
}

void View::update(const ViewUpdateData& data) {
    switch (ty)
    {
        case ViewType::Quad: {
            quad_draw.update(data);
            break;
        }
        case ViewType::MovingQuad: {
            moving_quad_draw.update(data);
            break;
        }
    }

    for (View& child : children) {
        child.update(data);
    }
}