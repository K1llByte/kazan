#ifndef COMMANDS_H
#define COMMANDS_H

#include <cstdint>

#include <utility>

namespace kzn
{

enum CmdType : uint16_t
{
    CMD_NONE = 0,
    CMD_SET,
    CMD_GPU,
};


template<CmdType T>
struct BasicCommand
{
    static constexpr CmdType type() noexcept { return T; }
};

// struct Command;

// template<CmdType T, typename D>
// struct CmdMaker
// {
//     template<typename... Args>
//     static struct Command make(Args&&... args) noexcept
//     {
//         return struct Command{
//             T,
//             D(std::forward<D>(args)...)
//         };
//     }
// };

////////////////////////////////////////
// Command data structs defenition

struct CmdSet
    : BasicCommand<CmdType::CMD_SET>
    // : CmdMaker<CmdType::CMD_SET, CmdSet>
{
    enum Target : uint16_t
    {
        CAMERA_POS,
        CAMERA_DIR,
        CAMERA_UP ,
    } target;

    CmdSet(const Target& _target)
        : target{_target} {}
};

////////////////////////////////////////

struct Command
{
    CmdType type;
    union CmdData
    {
        CmdSet cmd_set;
    } data;

    // template<typename D, typename... Args>
    // static Command make(Args&&... args)
    // {
    //     return Command{
    //         .type = D::type(),
    //         .data = { D(std::forward<D>(args)...) }
    //     };
    // }
};

template<typename D, typename... Args>
Command make(Args&&... args)
{
    return Command{
        .type = D::type(),
        .data = { D(std::forward<D>(args)...) }
    };
}

}

#endif // COMMANDS_H
