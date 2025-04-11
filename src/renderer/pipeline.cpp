#include "renderer.h"
#include "initializers.h"

#include "../util.h"

#include <vector>

Pipeline::Pipeline(
    const Renderer* renderer,
    PipelineID layout_id,
    VkSampleCountFlagBits sample_count,
    uint32_t color_attachment_count, const VkFormat* p_color_attachment_formats,
    bool alpha_blending
) {
    const ShaderData& vert_shader_data = renderer->get_shader_data(layout_id.vertex);
    const ShaderData& frag_shader_data = renderer->get_shader_data(layout_id.fragment);
    const VkPipelineLayout& layout = renderer->get_pipeline_layout(layout_id);

    std::vector<VkPipelineShaderStageCreateInfo> stages = {
        initializers::pipeline_shader_stage_create_info(vert_shader_data.shader_stage_bits(), vert_shader_data.shader_module),
        initializers::pipeline_shader_stage_create_info(frag_shader_data.shader_stage_bits(), frag_shader_data.shader_module)
    };

    VkVertexInputBindingDescription vertex_binding;
	std::vector<VkVertexInputAttributeDescription> vertex_attributes;
	VkPipelineVertexInputStateCreateInfo vertex_input_state;
    vert_shader_data.get_vertex_input_data(&vertex_binding, vertex_attributes, &vertex_input_state);

	VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };
	VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1
    };
	VkPipelineRasterizationStateCreateInfo rasterization_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .lineWidth = 1.0f
    };
	VkPipelineMultisampleStateCreateInfo multisample_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = sample_count
    };
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
    };
	VkPipelineColorBlendAttachmentState blend_attachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };
    if (alpha_blending) {
        blend_attachment.blendEnable = VK_TRUE;
        blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }
	VkPipelineColorBlendStateCreateInfo color_blend_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &blend_attachment
    };
	VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
	VkPipelineDynamicStateCreateInfo dynamic_state_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = ArrayCount(dynamic_states),
        .pDynamicStates = &dynamic_states[0]
    };
	VkPipelineRenderingCreateInfo rendering_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = color_attachment_count,
        .pColorAttachmentFormats = p_color_attachment_formats
    };
	VkGraphicsPipelineCreateInfo pipeline_ci = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &rendering_ci,
		.stageCount = 2,
		.pStages = stages.data(),
		.pVertexInputState = &vertex_input_state,
		.pInputAssemblyState = &input_assembly_state,
		.pViewportState = &viewport_state,
		.pRasterizationState = &rasterization_state,
		.pMultisampleState = &multisample_state,
		.pDepthStencilState = &depth_stencil_state,
		.pColorBlendState = &color_blend_state,
		.pDynamicState = &dynamic_state_ci,
		.layout = layout,
	};

    this->layout_id = layout_id;
	chk(vkCreateGraphicsPipelines(renderer->get_device(), VK_NULL_HANDLE, 1, &pipeline_ci, nullptr, &raw));
}

void Pipeline::destroy(VkDevice device) {
    vkDestroyPipeline(device, raw, nullptr);
}