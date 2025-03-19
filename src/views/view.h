#pragma once

#include <vector>
#include <memory>

#include <glm/mat4x4.hpp>

#include "../renderer/draw.h"

#include "../rect.h"

#include "shared.h"
#include "quad_draw.h"
#include "moving_quad_draw.h"
#include "color_quad_draw.h"

struct ViewDrawData {
    std::vector<float> vertices = {};
    std::vector<uint32_t> indices = {};
    std::vector<DrawCommand> draws = {};
};

struct Renderer;
struct View {
    View()
    {}

    template<class T>
    requires std::is_base_of_v<RenderableInterface, T>
    View(T renderable)
        : renderable(new T(std::move(renderable)))
    {}

    void push_child(View view)  {
        children.emplace_back(std::move(view));
    }

    void update(const ViewUpdateData& data);

    void append_draw_data(Renderer* renderer, ViewDrawData& data) const;
    ViewDrawData get_draw_data(Renderer* renderer);

    std::unique_ptr<RenderableInterface> renderable;

    std::vector<View> children;
};