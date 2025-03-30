#pragma once

#include <cstdint>
#include <functional>

// TODO: A lot of these IDs are still inneficient, can be better later
// TODO: Also way too much duplicated code, make macros for it or something
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

struct BufferID {
    BufferID() {}
    explicit BufferID(uint32_t id)
        : id(id)
    {}

    bool operator==(const BufferID &other) const {
        return id == other.id;
    }

    uint32_t id;
};

struct DescriptorSetLayoutID {
    DescriptorSetLayoutID() {}
    explicit DescriptorSetLayoutID(uint32_t id)
        : id(id)
    {}

    bool operator==(const DescriptorSetLayoutID &other) const {
        return id == other.id;
    }

    uint32_t id;
};

struct DescriptorSetID {
    DescriptorSetID() {}
    explicit DescriptorSetID(uint32_t id)
        : id(id)
    {}

    bool operator==(const DescriptorSetID &other) const {
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
    struct hash<BufferID>
    {
        size_t operator()(const BufferID& b) const
        {
            return make_hash(b.id);
        }
    };

    template<>
    struct hash<DescriptorSetLayoutID>
    {
        size_t operator()(const DescriptorSetLayoutID& d) const
        {
            return make_hash(d.id);
        }
    };

    template<>
    struct hash<DescriptorSetID>
    {
        size_t operator()(const DescriptorSetID& d) const
        {
            return make_hash(d.id);
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
}