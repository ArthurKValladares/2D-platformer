#pragma once

#include <vector>
#include <memory>

#include <glm/mat4x4.hpp>

#include "../renderer/draw.h"
#include "../rect.h"

#include "shared.h"

struct ViewDrawData {
    void upload_vertex_index_data(Renderer* renderer);
    
    std::vector<float> vertices = {};
    std::vector<uint32_t> indices = {};
    std::vector<DrawCommand> draws = {};
};

struct RenderableInterface {
    virtual bool is_empty() const = 0;
    virtual void update(const ViewUpdateData& data) = 0;
    virtual const ShaderPair& shaders() const = 0;
    virtual ShaderPair& shaders() = 0;
    virtual uint64_t vertex_data(std::vector<float>& vertex_buffer) = 0;
    virtual uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) = 0;
};

struct Renderer;
struct Renderable {
    Renderable()
    {}

    template<class T>
    requires std::is_base_of_v<RenderableInterface, T>
    Renderable(T renderable)
        : renderable(new T(std::move(renderable)))
    {}

    void push_child(Renderable renderable)  {
        children.emplace_back(std::move(renderable));
    }

    void update(const ViewUpdateData& data);

    void append_draw_data(Renderer* renderer, ViewDrawData& data) const;
    ViewDrawData get_draw_data(Renderer* renderer);

    // TODO: I don't like this shared_ptr, make it a raw pointer later?
    std::shared_ptr<RenderableInterface> renderable;

    std::vector<Renderable> children;
};