#pragma once

// Thanks to: https://stackoverflow.com/questions/19655733/how-to-use-unordered-set-that-has-elements-that-are-vector-of-pairint-int/19740245#19740245

// a little helper that should IMHO be standardized
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

// hash any container
template<typename T>
struct hash_container
{
    size_t operator()(const T& v) const
    {
        size_t h=0;
        for( const auto& e : v ) {
            hash_combine(h, make_hash(e));
        }
        return h;
    }
};

template<typename T, typename U>
struct HashPair
{
    size_t operator()(const std::pair<T,U>& v) const
    {
        size_t h=make_hash(v.first);
        hash_combine(h, make_hash(v.second));
        return h;
    }
};