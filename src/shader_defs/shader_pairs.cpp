#include "shader_pairs.h"

int32_t ShaderPair::push_descriptor_set_idx() const {
    const int32_t vert_idx = vertex->push_descriptor_set_idx();
    const int32_t frag_idx = fragment->push_descriptor_set_idx();
    if (vert_idx >= 0 && frag_idx >= 0) {
        assert(vert_idx == frag_idx);
    }
    return std::max(vert_idx, frag_idx);
}

void ShaderPair::append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const {
    vertex->append_push_descriptor_sets(sets);
    fragment->append_push_descriptor_sets(sets);
}

void ShaderPair::append_push_constant_data(std::vector<PushConstantData>& pcs) const {
    vertex->append_push_constant_data(pcs);
    fragment->append_push_constant_data(pcs);
}

uint32_t ShaderPair::vertex_num_floats() const {
    return vertex->vertex_num_floats();
}