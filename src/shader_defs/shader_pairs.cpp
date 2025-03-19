#include "shader_pairs.h"

uint32_t ShaderPair::vertex_num_floats() const {
    return vertex->vertex_num_floats();
}

DescriptorSetData ShaderPair::draw_texture_binding() const {
    return fragment->draw_texture_binding();
}

TextureSource ShaderPair::draw_texture() const {
    return fragment->draw_texture();
}

void ShaderPair::append_push_constant_data(std::vector<PushConstantData>& pcs) const {
    vertex->append_push_constant_data(pcs);
    fragment->append_push_constant_data(pcs);
}