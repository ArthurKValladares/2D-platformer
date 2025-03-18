#pragma once

#include <cstdint>
#include <functional>

// TODO: A lot of these IDs are still inneficient, can be better later
struct TextureID {
    TextureID() {}
    explicit TextureID(uint32_t id)
        : id(id)
    {}

    bool operator==(const TextureID &other) const {
        return id == other.id;
    }

    uint32_t id;
};

struct ShaderID {
    ShaderID() {}
    explicit ShaderID(uint32_t id)
        : id(id)
    {}

    bool operator==(const ShaderID &other) const {
        return id == other.id;
    }

    uint32_t id;
};

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

struct MaterialID {
    MaterialID() {}
    explicit MaterialID(TextureID texture, ShaderID vertex, ShaderID fragment)
        : texture(texture)
        , vertex(vertex)
        , fragment(fragment)
    {}

    bool operator==(const MaterialID &other) const {
        return texture == other.texture && vertex == other.vertex && fragment == other.fragment;
    }

    TextureID texture;
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
    template<>
    struct hash<TextureID>
    {
        size_t operator()(const TextureID& t) const
        {
            return make_hash(t.id);
        }
    };

    template<>
    struct hash<ShaderID>
    {
        size_t operator()(const ShaderID& t) const
        {
            return make_hash(t.id);
        }
    };

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

    template<>
    struct hash<MaterialID>
    {
        size_t operator()(const MaterialID& p) const
        {
            size_t h= make_hash(p.texture);
            hash_combine(h, make_hash(p.vertex));
            hash_combine(h, make_hash(p.fragment));
            return h;
        }
    };
}