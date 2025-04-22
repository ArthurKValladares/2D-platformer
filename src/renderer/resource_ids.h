#pragma once

#include <cstdint>
#include <functional>

#include "../hash.h"

#define CREATE_ID(NAME)\
struct NAME ## ID {\
    NAME ## ID() {}\
    explicit NAME ## ID(uint32_t id)\
        : id(id)\
    {}\
\
    bool operator==(const NAME ## ID &other) const {\
        return id == other.id;\
    }\
\
    uint32_t id;\
};

#define CREATE_HASH_FN(NAME)\
template<>\
struct hash<NAME ## ID>\
{\
    size_t operator()(const NAME ## ID& o) const\
    {\
        return make_hash(o.id);\
    }\
};

CREATE_ID(Texture)
CREATE_ID(Buffer)
CREATE_ID(DescriptorSetLayout)
CREATE_ID(DescriptorSet)
CREATE_ID(Shader)

struct PipelineID {
    PipelineID() {}
    explicit PipelineID(ShaderID vertex, ShaderID fragment, bool alpha_blending)
        : vertex(vertex)
        , fragment(fragment)
        , uses_alpha_blending(alpha_blending)
    {}

    bool operator==(const PipelineID &other) const {
        return vertex == other.vertex && fragment == other.fragment && uses_alpha_blending == other.uses_alpha_blending;
    }

    ShaderID vertex;
    ShaderID fragment;
    bool uses_alpha_blending;
};

namespace std
{
    CREATE_HASH_FN(Texture)
    CREATE_HASH_FN(Buffer)
    CREATE_HASH_FN(DescriptorSetLayout)
    CREATE_HASH_FN(DescriptorSet)
    CREATE_HASH_FN(Shader)

    template<>
    struct hash<PipelineID>
    {
        size_t operator()(const PipelineID& p) const
        {
            size_t h= make_hash(p.vertex);
            hash_combine(h, make_hash(p.fragment));
            hash_combine(h, make_hash(p.uses_alpha_blending));
            return h;
        }
    };
}