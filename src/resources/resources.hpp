#pragma once

#include "core/log.hpp"
#include "core/string_hash.hpp"
#include "fmt/format.h"
#include "resources/resource.hpp"

#include <filesystem>
#include <memory>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace kzn {

class Resources {
public:
    //! Find resource of specified type T, if not found, returns nullptr.
    //! \warning If specified type is not same type as the loaded resource,
    //! throws LoadingError.
    template<LoadableResource T>
    static std::shared_ptr<T> find(const std::filesystem::path& path) {
        auto it = s_resources.find(std::string_view{path.native()});
        if (it != s_resources.end()) {
            if (it->second.first != typeid(T)) {
                throw LoadingError{fmt::format(
                    "Incompatible specified resource type of '{}'",
                    path.native()
                )};
            }
            return std::static_pointer_cast<T>(it->second.second);
        }

        return nullptr;
    }

    //! Find or load resource of specified type T
    //! \warning If specified type is not same type as the loaded resource,
    //! throws LoadingError.
    template<LoadableResource T>
    static std::shared_ptr<T> find_or_load(const std::filesystem::path& path) {
        auto it = s_resources.find(std::string_view{path.native()});
        if (it != s_resources.end()) {
            if (it->second.first != typeid(T)) {
                throw LoadingError{fmt::format(
                    "Incompatible specified resource type of '{}'",
                    path.native()
                )};
            }
            return std::static_pointer_cast<T>(it->second.second);
        }

        auto [inserted_it, _] = s_resources.insert(
            {StringHash(path.native()),
             std::make_pair(
                 std::type_index(typeid(T)),
                 std::static_pointer_cast<void>(T::load(path))
             )}
        );

        Log::info("Loaded '{}'", path.native());

        return std::static_pointer_cast<T>(inserted_it->second.second);
    }

private:
    static inline std::unordered_map<
        StringHash,
        std::pair<std::type_index, std::shared_ptr<void>>>
        s_resources = {};
};

} // namespace kzn