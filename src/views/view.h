#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

#include "../renderer/draw.h"

#include "../rect.h"

#include "shared.h"
#include "quad_draw.h"
#include "moving_quad_draw.h"

struct ViewDrawData {
    std::vector<float> vertices = {};
    std::vector<uint32_t> indices = {};
    std::vector<DrawCommand> draws = {};
};

// TODO: this is not great, maybe use concepts if possible?
// IN other places too
enum class ViewType {
    Empty,
    Quad,
    MovingQuad,
};

struct Renderer;
struct View {
    View()
        : ty(ViewType::Empty)
    {}
    View(QuadDraw in_draw)
        : ty(ViewType::Quad)
        , quad_draw(in_draw)
    {}
    View(MovingQuadDraw in_draw)
        : ty(ViewType::MovingQuad)
        , moving_quad_draw(in_draw)
    {}

    void push_child(View view)  {
        children.push_back(view);
    }

    void update(const ViewUpdateData& data);

    void append_draw_data(Renderer* renderer, ViewDrawData& data) const;
    ViewDrawData get_draw_data(Renderer* renderer);

    ViewType ty;
    union{
        QuadDraw quad_draw;
        MovingQuadDraw moving_quad_draw;
    };

    std::vector<View> children;
};