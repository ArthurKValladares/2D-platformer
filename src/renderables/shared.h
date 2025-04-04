#pragma once

#include "../renderer/renderer.h"
#include "../keyboard_state.h"
#include "../shader_defs/shader_pairs.h"

struct ViewUpdateData {
    Renderer* renderer;
    double total_elapsed_seconds;
    double frame_dt;
    const KeyboardState& keyboard_state;
};

struct RenderableInterface {
    virtual bool is_empty() const = 0;
    virtual void update(const ViewUpdateData& data) = 0;
    virtual const ShaderPair& shaders() const = 0;
    virtual ShaderPair& shaders() = 0;
    virtual uint64_t vertex_data(std::vector<float>& vertex_buffer) = 0;
    virtual uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) = 0;
};