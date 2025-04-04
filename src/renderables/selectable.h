#pragma once

#include <memory>

#include "renderable.h"

struct SelectableRenderable {
    SelectableRenderable() {}

    void push_renderable(Renderable renderable)
    {
        renderables.emplace_back(std::move(renderable));
    }

    uint64_t size() const {
        return renderables.size();
    }

    uint64_t get_idx() const {
        return curr_idx;
    }
    
    void set_idx(uint64_t idx) {
        curr_idx = idx;
    }

    Renderable& get_curr_renderable() {
        return renderables[curr_idx];
    }

    std::vector<Renderable> renderables;
    uint64_t curr_idx = 0;
};