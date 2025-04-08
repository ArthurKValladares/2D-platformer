#include "renderable.h"
#include "../image.h"

#include "../renderer/renderer.h"

void Renderable::append_draw_data(Renderer* renderer, ViewDrawData& data) const {
    if (renderable && !renderable->is_empty()) {
        const ShaderPair&  shaders   = renderable->shaders();

        // Vertex shader data
        const ShaderSource vert_ty   = shaders.vertex->source();
        const ShaderID     vert_id   = shader_id(vert_ty);
        renderer->upload_shader(vert_id, shader_path(vert_ty));
        const ShaderData&  vert_data = renderer->get_shader_data(vert_id);

        // Fragment shader data
        const ShaderSource frag_ty   = shaders.fragment->source();
        const ShaderID     frag_id   = shader_id(frag_ty);
        renderer->upload_shader(frag_id, shader_path(frag_ty));
        const ShaderData&  frag_data = renderer->get_shader_data(frag_id);

        // Vertex/Index buffer data
        const uint32_t first_index   = data.indices.size();
        const uint32_t index_count   = renderable->index_data(data.vertices.size() / shaders.vertex_num_floats(), data.indices);
        const uint32_t _vertex_count = renderable->vertex_data(data.vertices);

        DrawCommand dc = DrawCommand{
            .vertex_id = vert_id,
            .fragment_id = frag_id,
            .index_count = index_count,
            .first_index = first_index
        };

        // Push Constant
        shaders.append_push_constant_data(dc.pcs);

        // Push Descriptor Set
        const int32_t push_set_idx = shaders.push_descriptor_set_idx();
        if (push_set_idx >= 0) {
            dc.push_set_idx = push_set_idx;
            shaders.append_push_descriptor_sets(dc.push_set_data);
            for (const PushDescriptorSetData& data : dc.push_set_data) {
                if (data.ty == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER && !renderer->contains_texture(data.texture_id)) {
                    const ImageData image_data = ImageData(texture_path(static_cast<TextureSource>(data.texture_id.id)));
                    renderer->upload_texture(data.texture_id, image_data.texture_create_info());
                }
            }
        }

        // Descriptor Sets
        dc.set_ids.push_back(DescriptorSetID(GLOBAL_DESCRIPTOR_SET_IDX));
        std::vector<DescriptorSetLayoutID> layout_ids = {
            DescriptorSetLayoutID(GLOBAL_DESCRIPTOR_SET_IDX)
        };
        const uint32_t max_set = std::max(vert_data.max_descriptor_set(), frag_data.max_descriptor_set());
        for (uint32_t layout_idx = GLOBAL_DESCRIPTOR_SET_IDX + 1; layout_idx <= max_set; ++layout_idx) {
            std::vector<VkDescriptorSetLayoutBinding> bindings = {};
            vert_data.append_layout_bindings_at(layout_idx, bindings);
            frag_data.append_layout_bindings_at(layout_idx, bindings);
            const DescriptorSetLayoutID layout_id = renderer->upload_descriptor_set_layout(bindings, VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR);
            layout_ids.push_back(layout_id);
        }

        // Upload draw
        renderer->upload_pipeline(vert_id, frag_id, layout_ids);
        data.draws.push_back(dc);
    }

    for (const Renderable& child : children) {
        child.append_draw_data(renderer, data);
    }
}

ViewDrawData Renderable::get_draw_data(Renderer* renderer) {
    ViewDrawData data = {};
    append_draw_data(renderer, data);

    return data;
}

void ViewDrawData::upload_vertex_index_data(Renderer* renderer) {
    if (!draws.empty()) {
        renderer->upload_index_data(&indices[0], indices.size() * sizeof(uint32_t));
        renderer->upload_vertex_data(&vertices[0], vertices.size() * sizeof(float));
    }
}