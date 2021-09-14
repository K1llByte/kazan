#ifndef COMMANDS_H
#define COMMANDS_H

#include <cstdint>
#include <cstring>

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
    static constexpr CmdType type = T;
    // static constexpr CmdType type() noexcept { return T; }
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

// Procedure to add a new command:
// 1. Create a struct with the command data
// that extends BasicCommand<CmdType>
//   1.1. Add at least the default ctor
// 2. Add struct to Command::CmdData union
// 3. Build the grammar needed for that command
// 4. Make the command handler in the controller

struct CmdNone
    : BasicCommand<CmdType::CMD_NONE>
{
    CmdNone() = default;
};

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

    CmdSet() = default;
    CmdSet(const Target& _target)
        : target{_target} {}
};

////////////////////////////////////////

struct Command
{
    CmdType type;
    union CmdData
    {
        CmdNone cmd_none;
        CmdSet cmd_set;
    } data;
};

template<typename D, typename... Args>
constexpr Command make(Args&&... args)
{
    // Command cmd{ .type = D::type };
    // // *reinterpret_cast<Command::CmdData*>(&cmd.data) = D(std::forward<D>(args)...);
    // auto d = D(std::forward<D>(args)...);
    // std::copy(&d, &d + 1, &cmd.data);
    // return cmd;

    Command cmd{
        .type = D::type,
        .data = {}
        };
    auto d = D(std::forward<D>(args)...);
    std::memcpy(&cmd.data, &d, sizeof(D));
    return cmd;

    // return Command{
    //     .type = D::type,
    //     .data = { D(std::forward<D>(args)...) }
    // };
}

}

#endif // COMMANDS_H
