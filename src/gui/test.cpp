#include "gui/console.hpp"

#include <fmt/core.h>

#include "utils/string.hpp"

#include <GLFW/glfw3.h>



// int main() {
//     using kzn::console::Commands;
//     using kzn::console::Arg;

//     Commands commands_tree;

//     {
//         using kzn::console::Int;
//         using kzn::console::Float;
//         using kzn::console::String;
//         commands_tree.add_command(
//             {"create", "sphere"},
//             {Float("radius"), Int("slices"), Int("stacks")},
//             [](Arg::Any* args) {
//                 float radius = std::get<float>(args[0]);
//                 int slices = std::get<int>(args[1]);
//                 int stacks = std::get<int>(args[2]);
//                 fmt::print("called create sphere {} {} {}\n", radius, slices, stacks);
//             }
//         );
//         commands_tree.add_command(
//             {"debug"},
//             [](Arg::Any*) {
//                 fmt::print("Debug command\n");
//             }
//         );
//     }

//     auto err = commands_tree.execute("create sphere 1 20 20");
//     fmt::print("{}\n", kzn::console::error_message(err));
// }

#include <type_traits>
#include <glm/glm.hpp>
#include <boost/pfr.hpp>

template< class T, class ...U>
inline constexpr bool is_any_v = (std::is_same_v<T, U> || ...);

template<typename T>
constexpr bool is_uniform()
    requires std::is_default_constructible<T>::value
{
    bool res = true;
    boost::pfr::for_each_field(std::forward<T>(T{}), [&res]<typename Field>(Field&) {
        res = is_any_v<
            Field,
            bool,
            int,
            uint,
            float,
            double,
            glm::bvec2, glm::bvec3, glm::bvec4,
            glm::ivec2, glm::ivec3, glm::ivec4,
            glm::uvec2, glm::uvec3, glm::uvec4,
            glm::vec2,  glm::vec3,  glm::vec4,
            glm::dvec2, glm::dvec3, glm::dvec4,
            glm::mat2,    glm::mat3,   glm::mat4,
            glm::mat2x3,  glm::mat2x4,
            glm::mat3x2,  glm::mat3x4,
            glm::mat4x2,  glm::mat4x3,
            glm::dmat2,   glm::dmat3,  glm::dmat4,
            glm::dmat2x3, glm::dmat2x4,
            glm::dmat3x2, glm::dmat3x4,
            glm::dmat4x2, glm::dmat4x3>;
        });
    return res;
}

struct Bar;

struct Foo {
    glm::vec3 a;
    glm::vec3 b;
};

int main() {
    constexpr size_t counter = count_fields<Foo>();
    fmt::print("{}\n", counter);
}