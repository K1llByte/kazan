#pragma once

#include "core/assert.hpp"
#include "core/flat_map.hpp"
#include "core/string_hash.hpp"

#include <filesystem>
#include <optional>

namespace kzn {

//! Provides a lightweight alias mapping system.
class PathAliases {
public:
    // Ctor
    PathAliases() = default;
    // Dtor
    ~PathAliases() = default;

    //! Returns true if this alias was not duplicate, otherwise it wasn't created.
    bool add(StringHash alias_hash, std::filesystem::path path) {
        if(m_path_aliases.contains(alias_hash)) {
            return false;
        }

        m_path_aliases.emplace(alias_hash, std::move(path));
        return true;
    }

    //! Returns true if this alias was removed, otherwise false.
    bool remove(StringHash alias_hash) {
        return m_path_aliases.erase(alias_hash);
    }

    //! Returns alias path.
    [[nodiscard]]
    std::filesystem::path get(StringHash alias_hash) const {
        KZN_ASSERT_MSG(
            m_path_aliases.contains(alias_hash),
            "Path alias does not exist"
        );
        auto path = m_path_aliases.find(alias_hash);
        return path->second;
    }

    //! Returns optional alias path.
    [[nodiscard]]
    std::optional<std::filesystem::path> try_get(StringHash alias_hash) const {
        auto path = m_path_aliases.find(alias_hash);
        if(path != m_path_aliases.end()) {
            return path->second;
        }

        return std::nullopt;
    }


    //! Resolve a path with alias.
    [[nodiscard]]
    std::optional<std::filesystem::path> resolve(const std::string_view path) const {
        constexpr std::string_view alias_end_token = "://";
        constexpr auto alias_end_token_size = alias_end_token.size();

        // Find alias token "://" in path. If token not found, there is no
        // alias, return original path.
        const auto token_pos = path.find(alias_end_token);
        if(token_pos == path.npos) {
            return path;
        }

        // Find alias path in map, if it doesn't exist return nullopt.
        const auto alias = path.substr(0, token_pos);
        auto alias_path = m_path_aliases.find(alias);
        if(alias_path == m_path_aliases.end()) {
            return std::nullopt;
        }

        const auto relative_path = path.substr(token_pos + alias_end_token.size());
        return alias_path->second / relative_path;
    }

private:
    FlatMap<StringHash, std::filesystem::path> m_path_aliases;
};

} // namespace kzn

