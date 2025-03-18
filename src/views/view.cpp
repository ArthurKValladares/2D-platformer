#include "view.h"
#include "../image.h"

#include "../renderer/renderer.h"

void View::append_draw_data(Renderer* renderer, ViewDrawData& data) const {
    if (renderable && !renderable->is_empty()) {
        const ShaderPair& shaders = renderable->shaders();

        const TextureSource draw_texture = shaders.draw_texture();
        const uint32_t first_index = data.indices.size();

        const uint32_t index_count = renderable->index_data(data.vertices.size() / shaders.vertex_num_floats(), data.indices);
        renderable->vertex_data(data.vertices);

        DrawCommand dc = DrawCommand{
            .texture_id = texture_id(draw_texture),
            .vertex_id = shader_id(shaders.vertex_ty),
            .fragment_id = shader_id(shaders.fragment_ty),
            .index_count = index_count,
            .first_index = first_index
        };
        shaders.append_push_constant_data(dc.pcs);
        data.draws.push_back(dc);

        // Upload draw data
        const ImageData image_data = ImageData(texture_path(draw_texture));
        renderer->upload_texture(texture_id(draw_texture), image_data.texture_create_info());

        renderer->upload_shader(shader_id(shaders.vertex_ty), shader_path(shaders.vertex_ty));
        renderer->upload_shader(shader_id(shaders.fragment_ty), shader_path(shaders.fragment_ty));

        renderer->upload_pipeline(shader_id(shaders.vertex_ty), shader_id(shaders.fragment_ty));

        renderer->upload_material(
            texture_id(draw_texture),
            shader_id(shaders.vertex_ty),
            shader_id(shaders.fragment_ty),
            shaders.draw_texture_binding().set
        );
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
    if (renderable) {
        renderable->update(data);
    }

    for (View& child : children) {
        child.update(data);
    }
}