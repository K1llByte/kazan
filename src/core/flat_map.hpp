#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <functional>


// A sorted VectorMap with binary search gives you most of the benefits of
// std::map / flat_map with better cache locality and zero node
// allocations. Inspired by boost::collections::flat_map.
template<
    typename Key,
    typename T,
    typename Compare = std::less<Key>,
    typename Allocator = std::allocator<std::pair<Key, T>>
>
class FlatMap {
public:
    using value_type     = std::pair<Key, T>;
    using container_type = std::vector<value_type, Allocator>;
    using iterator       = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using size_type      = typename container_type::size_type;

    // ------------------------------------------------------------
    // Constructors
    // ------------------------------------------------------------
    FlatMap() = default;

    explicit FlatMap(const Compare& comp,
                             const Allocator& alloc = Allocator{})
        : m_data(alloc), m_comp(comp) {}

    // ------------------------------------------------------------
    // Iterators
    // ------------------------------------------------------------
    iterator begin() noexcept { return m_data.begin(); }
    const_iterator begin() const noexcept { return m_data.begin(); }
    iterator end() noexcept { return m_data.end(); }
    const_iterator end() const noexcept { return m_data.end(); }

    // ------------------------------------------------------------
    // Capacity
    // ------------------------------------------------------------
    bool empty() const noexcept { return m_data.empty(); }
    size_type size() const noexcept { return m_data.size(); }
    void reserve(size_type n) { m_data.reserve(n); }

    // ------------------------------------------------------------
    // Lookup
    // ------------------------------------------------------------
    iterator find(const Key& key) {
        auto it = lower_bound(key);
        if (it != end() && keys_equal(it->first, key)) {
            return it;
        }
        return end();
    }

    const_iterator find(const Key& key) const {
        auto it = lower_bound(key);
        if (it != end() && keys_equal(it->first, key)) {
            return it;
        }
        return end();
    }

    bool contains(const Key& key) const {
        return find(key) != end();
    }

    T& at(const Key& key) {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range("SortedVectorMap::at");
        }
        return it->second;
    }

    const T& at(const Key& key) const {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range("SortedVectorMap::at");
        }
        return it->second;
    }

    // ------------------------------------------------------------
    // Modifiers
    // ------------------------------------------------------------
    std::pair<iterator, bool> insert(const value_type& value) {
        auto it = lower_bound(value.first);
        if (it != end() && keys_equal(it->first, value.first)) {
            return {it, false};
        }
        it = m_data.insert(it, value);
        return {it, true};
    }

    std::pair<iterator, bool> insert(value_type&& value) {
        auto it = lower_bound(value.first);
        if (it != end() && keys_equal(it->first, value.first)) {
            return {it, false};
        }
        it = m_data.insert(it, std::move(value));
        return {it, true};
    }

    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        value_type value(std::forward<Args>(args)...);
        return insert(std::move(value));
    }

    bool erase(const Key& key) {
        auto it = find(key);
        if (it == end()) {
            return false;
        }
        m_data.erase(it);
        return true;
    }

    void clear() noexcept {
        m_data.clear();
    }

    // ------------------------------------------------------------
    // Element access
    // ------------------------------------------------------------
    T& operator[](const Key& key) {
        auto it = lower_bound(key);
        if (it != end() && keys_equal(it->first, key)) {
            return it->second;
        }
        it = m_data.insert(it, value_type(key, T{}));
        return it->second;
    }

    // ------------------------------------------------------------
    // Observers
    // ------------------------------------------------------------
    Compare key_comp() const {
        return m_comp;
    }

private:
    container_type m_data;
    Compare m_comp{};

    iterator lower_bound(const Key& key) {
        return std::lower_bound(
            m_data.begin(),
            m_data.end(),
            key,
            [&](const value_type& v, const Key& k) {
                return m_comp(v.first, k);
            }
        );
    }

    const_iterator lower_bound(const Key& key) const {
        return std::lower_bound(
            m_data.begin(),
            m_data.end(),
            key,
            [&](const value_type& v, const Key& k) {
                return m_comp(v.first, k);
            }
        );
    }

    bool keys_equal(const Key& a, const Key& b) const {
        return !m_comp(a, b) && !m_comp(b, a);
    }
};
