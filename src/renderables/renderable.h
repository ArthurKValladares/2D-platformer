#pragma once

#include <vector>
#include <memory>

#include <glm/mat4x4.hpp>

#include "../renderer/draw.h"
#include "../rect.h"
#include "../renderer/renderer.h"
#include "../shader_defs/shader_pairs.h"

struct ViewDrawData {
    void upload_vertex_index_data(Renderer* renderer);
    
    DescriptorSetLayoutID global_layout_id;
    DescriptorSetID global_set_id;

    std::vector<float> vertices = {};
    std::vector<uint32_t> indices = {};
    std::vector<DrawCommand> draws = {};
};

struct RenderableInterface {
    virtual bool is_empty() const = 0;
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
    Renderable(T renderable, bool alpha_blending = false)
        : renderable(new T(std::move(renderable)))
        , uses_alpha_blending(alpha_blending)
    {}

    void push_child(Renderable renderable)  {
        children.emplace_back(std::move(renderable));
    }

    void append_draw_data(Renderer* renderer, ViewDrawData& data) const;
    ViewDrawData get_draw_data(Renderer* renderer, DescriptorSetLayoutID global_layout_id, DescriptorSetID global_set_id);

    // TODO: I don't like this shared_ptr, make it a raw pointer later?
    // Or it could be even better if i turned this into an index and made it more of an
    // ECS-like thing, would also help with some other stuff I think.
    // This whole Renderable idea is not great atm, re-think it
    std::shared_ptr<RenderableInterface> renderable;
    // TODO: I'm putting the alpha-blending info here for now, not sure this is the right way to do it
    bool uses_alpha_blending;

    std::vector<Renderable> children;
};