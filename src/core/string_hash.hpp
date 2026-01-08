#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <string_view>

namespace kzn {

namespace internal {

//! https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
template<typename T>
struct Fnv1Params;

template<>
struct Fnv1Params<std::uint32_t> {
    static constexpr std::uint32_t offset = 2166136261;
    static constexpr std::uint32_t prime = 16777619;
};

template<>
struct Fnv1Params<std::uint64_t> {
    static constexpr std::uint64_t offset = 14695981039346656037ull;
    static constexpr std::uint64_t prime = 1099511628211ull;
};

} // namespace internal

template<typename T>
class BasicStringHash {
public:
    using value_type = char;

public:
    //! Constructor of string hash with a c-style char array string.
    //! \note Does not take into account the null terminator (last character).
    template<size_t N>
    BasicStringHash(const value_type (&str)[N])
        : BasicStringHash(std::string_view{str, N - 1}) {}

    //! Constructor of string hash with string.
    //! \note Assumes string does not include null terminator
    BasicStringHash(const std::string& str)
        : BasicStringHash(std::string_view{str}) {}

    //! Constructor of string hash with string_view.
    //! \note Assumes string_view does not include null terminator
    BasicStringHash(const std::string_view str)
        : m_hash{std::ranges::fold_left(
              str,
              internal::Fnv1Params<T>::offset,
              [](T accum, char c) {
                  return accum * internal::Fnv1Params<T>::prime ^
                         std::uint8_t(c);
              }
          )} {}

    // Copy
    BasicStringHash(const BasicStringHash&) = default;
    BasicStringHash& operator=(const BasicStringHash&) = default;
    // Move
    BasicStringHash(BasicStringHash&&) = default;
    BasicStringHash& operator=(BasicStringHash&&) = default;
    // Dtor
    ~BasicStringHash() = default;

    operator std::uint32_t() const { return m_hash; }

public:
    T m_hash;
};

using StringHash = BasicStringHash<std::uint64_t>;

} // namespace kzn

namespace std {

template<typename T>
struct hash<kzn::BasicStringHash<T>> {
    std::uint32_t operator()(const kzn::BasicStringHash<T>& s) const noexcept {
        return std::hash<T>{}(s.m_hash);
    }
};

} // namespace std