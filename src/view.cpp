#include "view.h"

#include "renderer/renderer.h"

void View::append_draw_data(Renderer* renderer, ViewDrawData& data) const {
    if (!draw.is_empty()) {
        const uint32_t first_index = data.indices.size();

        const uint32_t index_count = draw.rect.index_data(data.vertices.size(), data.indices);
        draw.rect.vertex_data(data.vertices);

        // TODO: I need a better way to get the actual material I want
        data.draws.push_back(DrawCommand{
            .material_idx = static_cast<uint64_t>(draw.texture),
            .index_count = index_count,
            .first_index = first_index
        });
    }

    for (const View& child : children) {
        child.append_draw_data(renderer, data);
    }
}