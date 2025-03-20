#include "view.h"
#include "../image.h"

#include "../renderer/renderer.h"

void View::append_draw_data(Renderer* renderer, ViewDrawData& data) const {
    if (renderable && !renderable->is_empty()) {
        const ShaderPair& shaders = renderable->shaders();
        const ShaderSource vertex_ty = shaders.vertex->source();
        const ShaderSource fragment_ty = shaders.fragment->source();

        const uint32_t first_index = data.indices.size();

        const uint32_t index_count = renderable->index_data(data.vertices.size() / shaders.vertex_num_floats(), data.indices);
        renderable->vertex_data(data.vertices);

        DrawCommand dc = DrawCommand{
            .vertex_id = shader_id(vertex_ty),
            .fragment_id = shader_id(fragment_ty),
            .index_count = index_count,
            .first_index = first_index
        };
        shaders.append_push_constant_data(dc.pcs);
        data.draws.push_back(dc);

        // Upload draw data
        shaders.append_descriptor_sets(dc.sets);
        for (const DescriptorSetData& data : dc.sets) {
            if (data.ty == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                // TODO: Its inefficient that i'm reacreating the ImageData even if the texture already exists.
                // add a way to check that publicly to the renderer
                const ImageData image_data = ImageData(texture_path(static_cast<TextureSource>(data.texture_id.id)));
                renderer->upload_texture(data.texture_id, image_data.texture_create_info());
            }
        }

        renderer->upload_shader(shader_id(vertex_ty), shader_path(vertex_ty));
        renderer->upload_shader(shader_id(fragment_ty), shader_path(fragment_ty));

        renderer->upload_pipeline(shader_id(vertex_ty), shader_id(fragment_ty));
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