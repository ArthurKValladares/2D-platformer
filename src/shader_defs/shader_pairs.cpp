#include "shader_pairs.h"

void ShaderPair::append_descriptor_sets(std::vector<DescriptorSetData>& sets) const {
    vertex->append_descriptor_sets(sets);
    fragment->append_descriptor_sets(sets);
}

void ShaderPair::append_push_constant_data(std::vector<PushConstantData>& pcs) const {
    vertex->append_push_constant_data(pcs);
    fragment->append_push_constant_data(pcs);
}

uint32_t ShaderPair::vertex_num_floats() const {
    return vertex->vertex_num_floats();
}