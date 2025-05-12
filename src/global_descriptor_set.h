#pragma once

#include "renderer/renderer.h"
#include "camera.h"

struct GlobalDescriptorSetData {
    GlobalDescriptorSetData(Renderer*renderer, const OrthographicCamera& camera) 
        : layout_id(renderer->upload_descriptor_set_layout(get_global_set_bindings()))
        , set_id(renderer->upload_descriptor_set(layout_id))
        , shader_data(GlobalShaderData{
            .proj_matrix = camera.get_proj_matrix()
        })
        , buffer_id(renderer->request_buffer(
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU,
            sizeof(GlobalShaderData)
        ))
    {}

    void write_shader_data_to_buffer(Renderer* renderer) {
        Buffer& buffer = renderer->get_buffer(buffer_id);
        buffer.write_to(&shader_data, sizeof(GlobalShaderData));
    }

    DescriptorSetLayoutID layout_id;
    DescriptorSetID set_id;
    GlobalShaderData shader_data;
    BufferID buffer_id;
};