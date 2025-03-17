#include "view.h"
#include "image.h"

#include "renderer/renderer.h"

void View::append_draw_data(Renderer* renderer, ViewDrawData& data) const {
    if (!draw.is_empty()) {
        const TextureSource draw_texture = draw.shaders.draw_texture();
        const uint32_t first_index = data.indices.size();

        const uint32_t index_count = draw.rect.index_data(data.vertices.size() / draw.shaders.vertex_num_floats(), data.indices);
        draw.rect.vertex_data(data.vertices);

        DrawCommand dc = DrawCommand{
            .texture_id = static_cast<uint64_t>(draw_texture),
            .pipeline_id = std::make_pair(
                static_cast<uint32_t>(draw.shaders.vertex_ty),
                static_cast<uint32_t>(draw.shaders.fragment_ty)
            ),
            .index_count = index_count,
            .first_index = first_index
        };
        draw.shaders.append_push_constant_data(dc.pcs);
        data.draws.push_back(dc);

        // Upload draw data
        const ImageData image_data = ImageData(texture_path(draw_texture));
        renderer->upload_texture(static_cast<uint32_t>(draw_texture), image_data.texture_create_info());

        renderer->upload_shader(static_cast<uint32_t>(draw.shaders.vertex_ty), shader_path(draw.shaders.vertex_ty));
        renderer->upload_shader(static_cast<uint32_t>(draw.shaders.fragment_ty), shader_path(draw.shaders.fragment_ty));

        renderer->upload_pipeline(static_cast<uint32_t>(draw.shaders.vertex_ty), static_cast<uint32_t>(draw.shaders.fragment_ty));

        renderer->upload_material(
            static_cast<uint32_t>(draw_texture),
            static_cast<uint32_t>(draw.shaders.vertex_ty),
            static_cast<uint32_t>(draw.shaders.fragment_ty),
            draw.shaders.draw_texture_binding().set
        );
    }

    for (const View& child : children) {
        child.append_draw_data(renderer, data);
    }
}