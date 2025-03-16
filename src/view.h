#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

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
    View(QuadDraw in_draw,  bool use_pc = false, glm::mat4 transform = glm::mat4(1.0))
        : draw(in_draw)
        , use_pc(use_pc)
        , transform(transform)
    {}

    void push_child(View view)  {
        children.push_back(view);
    }

    void append_draw_data(Renderer* renderer, ViewDrawData& data) const;

    QuadDraw draw;

    // TODO: This use_pc stuff is very bad, very temp
    bool use_pc;
    glm::mat4 transform;

    std::vector<View> children;
};