#pragma once

#include <vector>

#include "renderer/draw.h"

#include "rect.h"
#include "quad_draw.h"

// TODO: Right now Views are hard-coded to be quads, will be more generic later
struct ViewDrawData {
    std::vector<QuadVertex> vertices = {};
    std::vector<uint32_t> indices = {};
    std::vector<DrawCommand> draws = {};
};

struct Renderer;
struct View {
    View() {}
    View(QuadDraw in_draw)
        : draw(in_draw)
    {}

    void push_child(View view)  {
        children.push_back(view);
    }

    void append_draw_data(Renderer* renderer, ViewDrawData& data) const;

    QuadDraw draw;

    std::vector<View> children;
};