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

class ResourceCache {
public:
    PathAliases path_aliases;

public:
    // Ctor
    ResourceCache() = default;
    // Dtor
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
        auto it = m_resources.find(std::string_view{resolved_path.native()});
        if (it != m_resources.end()) {
            if (it->second.first != typeid(T)) {
                throw LoadingError{fmt::format(
                    "Incompatible specified resource type of '{}'",
                    resolved_path.native()
                )};
            }
            return std::static_pointer_cast<T>(it->second.second);
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
        auto it = m_resources.find(std::string_view{resolved_path.native()});
        if (it != m_resources.end()) {
            if (it->second.first != typeid(T)) {
                throw LoadingError{fmt::format(
                    "Incompatible specified resource type of '{}'",
                    resolved_path.native()
                )};
            }
            return std::static_pointer_cast<T>(it->second.second);
        }

        auto [inserted_it, _] = m_resources.insert(
            {StringHash(resolved_path.native()),
             std::make_pair(
                 std::type_index(typeid(T)),
                 std::static_pointer_cast<void>(T::load(resolved_path.native()))
             )}
        );

        Log::info("Loaded '{}'", path);

        return std::static_pointer_cast<T>(inserted_it->second.second);
    }

private:
    std::unordered_map<
        StringHash,
        std::pair<std::type_index, std::shared_ptr<void>>>
        m_resources;
};

// NOTE: This will be a global for now, but in the future, application should
// own the instance of the resource loader.
inline ResourceCache g_resources = {};

} // namespace kzn