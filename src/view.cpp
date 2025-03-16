#include "view.h"

#include "renderer/renderer.h"

void View::append_draw_data(Renderer* renderer, ViewDrawData& data) const {
    if (!draw.is_empty()) {
        const uint32_t first_index = data.indices.size();

        const uint32_t index_count = draw.rect.index_data(data.vertices.size(), data.indices);
        draw.rect.vertex_data(data.vertices);

        TextureSource draw_texture = draw.shaders.draw_texture();
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
    }

    for (const View& child : children) {
        child.append_draw_data(renderer, data);
    }
}