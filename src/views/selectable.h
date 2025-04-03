#pragma once

#include <memory>

#include "renderable.h"

struct SelectableView final : RenderableInterface {
    SelectableView() {}

    template<class T>
    requires std::is_base_of_v<RenderableInterface, T>
    void push_renderable(T renderable)
    {
        views.push_back(new T(std::move(renderable)));
    }

    bool is_empty() const {
        return views.empty();
    }

    uint64_t size() const {
        return views.size();
    }

    void set_idx(uint32_t idx) {
        curr_view = idx;
    }

    void update(const ViewUpdateData& data) {
        views[curr_view]->update(data);
    }

    const ShaderPair& shaders() const {
        return views[curr_view]->shaders();
    }

    ShaderPair& shaders() {
        return views[curr_view]->shaders();
    }

    uint64_t vertex_data(std::vector<float>& vertex_buffer) {
        return views[curr_view]->vertex_data(vertex_buffer);
    }

    uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) {
        return views[curr_view]->index_data(vertex_offset, index_buffer);
    }

    std::vector<std::unique_ptr<RenderableInterface>> views;
    uint32_t curr_view = 0;
};