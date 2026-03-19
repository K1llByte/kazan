#pragma once

#include "core/log.hpp"
#include "core/string_hash.hpp"
#include "fmt/format.h"
#include "resources/path_aliases.hpp"
#include "resources/resource.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace kzn {

struct ResourceKeyHash {
    std::size_t operator()(const std::pair<StringHash, std::type_index>& k) const {
        std::size_t h1 = std::hash<StringHash>{}(k.first);
        std::size_t h2 = std::hash<std::type_index>{}(k.second);
        return h1 ^ (h2 << 1);
    }
};

class ResourceCache {
public:
    PathAliases path_aliases;

public:
    ResourceCache() = default;
    ~ResourceCache() = default;

    //! Find resource of specified type T, if not found, returns nullptr.
    //! \warning If specified type is not same type as the loaded resource,
    //! or if path contains a path alias that wasn't registered, throws
    // LoadingError.
    template<LoadableResource T>
    std::shared_ptr<T> find(const std::string_view path) const {
        const auto resolved_path_opt = path_aliases.resolve(path);
        // If path contains a path alias that wasn't registered.
        if (resolved_path_opt == std::nullopt) {
            throw LoadingError{fmt::format("Unknown path alias '{}'", path)};
        }

        const auto& resolved_path = resolved_path_opt.value();
        const auto key = std::make_pair(
            StringHash(std::string_view{resolved_path.native()}),
            std::type_index(typeid(T))
        );
        auto it = m_resources.find(key);
        if (it != m_resources.end()) {
            return std::static_pointer_cast<T>(it->second);
        }

        return nullptr;
    }

    //! Find or load resource of specified type T
    //! \warning If specified type is not same type as the loaded resource,
    //! or if path contains a path alias that wasn't registered, throws
    // LoadingError.
    template<LoadableResource T>
    std::shared_ptr<T> load(const std::string_view path) {
        const auto resolved_path_opt = path_aliases.resolve(path);
        // If path contains a path alias that wasn't registered.
        if (resolved_path_opt == std::nullopt) {
            throw LoadingError{fmt::format("Unknown path alias '{}'", path)};
        }

        const auto& resolved_path = resolved_path_opt.value();
        const auto key = std::make_pair(
            StringHash(std::string_view{resolved_path.native()}),
            std::type_index(typeid(T))
        );
        auto it = m_resources.find(key);
        if (it != m_resources.end()) {
            return std::static_pointer_cast<T>(it->second);
        }

        auto [inserted_it, _] = m_resources.insert({
            key,
            std::static_pointer_cast<void>(T::load(resolved_path.native()))
        });

        Log::info("Loaded '{}'", path);

        return std::static_pointer_cast<T>(inserted_it->second);
    }

private:
    std::unordered_map<
        std::pair<StringHash, std::type_index>,
        std::shared_ptr<void>,
        ResourceKeyHash
    > m_resources;
};

// NOTE: This will be a global for now, but in the future, application should
// own the instance of the resource loader.
inline ResourceCache g_resources = {};

} // namespace kzn