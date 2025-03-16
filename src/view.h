#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

#include "renderer/draw.h"

#include "rect.h"
#include "quad_draw.h"

struct ViewDrawData {
    std::vector<float> vertices = {};
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