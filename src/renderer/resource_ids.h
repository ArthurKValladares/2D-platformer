#pragma once

#include <cstdint>
#include <functional>

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

// TODO: A lot of these IDs are still inneficient, can be better later
// TODO: Also way too much duplicated code, make macros for it or something
CREATE_ID(Texture)
CREATE_ID(Buffer)
CREATE_ID(DescriptorSetLayout)
CREATE_ID(DescriptorSet)
CREATE_ID(Shader)

struct PipelineID {
    PipelineID() {}
    explicit PipelineID(ShaderID vertex, ShaderID fragment)
        : vertex(vertex)
        , fragment(fragment)
    {}

    bool operator==(const PipelineID &other) const {
        return vertex == other.vertex && fragment == other.fragment;
    }

    ShaderID vertex;
    ShaderID fragment;
};

template<typename T>
std::size_t make_hash(const T& v)
{
    return std::hash<T>()(v);
}

// adapted from boost::hash_combine
inline void hash_combine(std::size_t& h, const std::size_t& v)
{
    h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2);
}

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
            return h;
        }
    };
}