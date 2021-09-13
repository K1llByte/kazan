#include "commands.hpp"

#include <utility>

namespace kzn
{

// template<typename D, typename... Args>
// Command Command::make(Args&&... args)
// {
//     return Command{
//         .type = D::type(),
//         .data = { D(std::forward<D>(args)...) }
//     };
// }


// template<CmdType T, typename D>
// template<typename... Args>
// Command CmdMaker<T,D>::make(Args&&... args)
// {
//     return Command{
//         T,
//         D(std::forward<D>(args)...)
//     };
// }

}
