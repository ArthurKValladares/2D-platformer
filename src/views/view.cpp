#include "view.h"
#include "../image.h"

#include "../renderer/renderer.h"

void View::append_draw_data(Renderer* renderer, ViewDrawData& data) const {
    if (renderable && !renderable->is_empty()) {
        const ShaderPair&  shaders   = renderable->shaders();

        const ShaderSource vert_ty   = shaders.vertex->source();
        const ShaderID     vert_id   = shader_id(vert_ty);
        renderer->upload_shader(vert_id, shader_path(vert_ty));
        const ShaderData&  vert_data = renderer->get_shader_data(vert_id);

        const ShaderSource frag_ty   = shaders.fragment->source();
        const ShaderID     frag_id   = shader_id(frag_ty);
        renderer->upload_shader(frag_id, shader_path(frag_ty));
        const ShaderData&  frag_data = renderer->get_shader_data(frag_id);

        const uint32_t first_index = data.indices.size();
        const uint32_t index_count = renderable->index_data(data.vertices.size() / shaders.vertex_num_floats(), data.indices);
        renderable->vertex_data(data.vertices);

        DrawCommand dc = DrawCommand{
            .vertex_id = vert_id,
            .fragment_id = frag_id,
            .index_count = index_count,
            .first_index = first_index
        };

        shaders.append_push_constant_data(dc.pcs);

        shaders.append_push_descriptor_sets(dc.push_set_data);
        for (const PushDescriptorSetData& data : dc.push_set_data) {
            if (data.ty == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER && !renderer->contains_texture(data.texture_id)) {
                const ImageData image_data = ImageData(texture_path(static_cast<TextureSource>(data.texture_id.id)));
                renderer->upload_texture(data.texture_id, image_data.texture_create_info());
            }
        }

        // TODO: I'm hard-coding the layout id for the global set to be 0 for now, not necessarily true
        std::vector<DescriptorSetLayoutID> layout_ids = {DescriptorSetLayoutID(0)};
        const uint32_t max_set = std::max(vert_data.max_descriptor_set(), frag_data.max_descriptor_set());
        for (uint32_t layout_idx = GLOBAL_DESCRIPTOR_SET_IDX + 1; layout_idx <= max_set; ++layout_idx) {
            std::vector<VkDescriptorSetLayoutBinding> bindings = {};
            vert_data.append_layout_bindings_at(layout_idx, bindings);
            frag_data.append_layout_bindings_at(layout_idx, bindings);
            const DescriptorSetLayoutID layout_id = renderer->upload_descriptor_set_layout(bindings, VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR);
            layout_ids.push_back(layout_id);
        }

        renderer->upload_pipeline(vert_id, frag_id, layout_ids);

        data.draws.push_back(dc);
    }

    for (const View& child : children) {
        child.append_draw_data(renderer, data);
    }
}

ViewDrawData View::get_draw_data(Renderer* renderer) {
    ViewDrawData data = {};
    append_draw_data(renderer, data);

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

void ViewDrawData::upload_vertex_index_data(Renderer* renderer) {
    if (!draws.empty()) {
        renderer->upload_index_data(&indices[0], indices.size() * sizeof(uint32_t));
        renderer->upload_vertex_data(&vertices[0], vertices.size() * sizeof(float));
    }
}