#pragma once

#include "util.h"

template<typename T>
uint64_t make_hash(const T& v)
{
    return std::hash<T>()(v);
}

// adapted from boost::hash_combine
inline void hash_combine(uint64_t& h, const uint64_t& v)
{
    h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2);
}

namespace std
{
    template<class T, class U>
    struct hash<std::pair<T, U>>
    {
        size_t operator()(const std::pair<T, U>& p) const
        {
            size_t h = make_hash(p.first);
            hash_combine(h, make_hash(p.second));
            return h;
        }
    };
}