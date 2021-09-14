#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "commands.hpp"

#include "pegtl.hpp"

/* 

Grammar : Cmd EOF

Target : TARGET

Vec3 : '(' Float ',' Float ',' Float ')'

Value : Vec3
      | Float

Cmd : CMD_SET Target Value
    | CMD_GPU Target Value

*/

namespace kzn
{

namespace pegtl = tao::pegtl;

////////// Start Of Grammar //////////

using spaces = pegtl::plus<pegtl::one<' '>>;
using ignored = pegtl::star<pegtl::one<' '>>;
template<typename T>
using command = pegtl::seq<ignored, T, ignored>;

using set = TAO_PEGTL_STRING("set");
using camera_pos = TAO_PEGTL_STRING("camera.pos"); // pegtl::string<'c','a','m','e','r','a','.','p','o','s'>;
using camera_dir = TAO_PEGTL_STRING("camera.dir"); // pegtl::string<'c','a','m','e','r','a','.','d','i','r'>;


struct name
    : pegtl::plus<pegtl::alpha>
{};

struct set_cam_pos
    : pegtl::seq<set, spaces, camera_pos>
{};

struct set_cam_dir
    : pegtl::seq<set, spaces, camera_dir>
{};

struct grammar
    // : pegtl::must<
    //     pegtl::sor<
    //         set_cam_pos,
    //         set_cam_dir
    //         >, pegtl::eof>
    : pegtl::seq<
        pegtl::sor<
            command<set_cam_pos>,
            command<set_cam_dir>
            >, pegtl::eof>
{};

/////////// End Of Grammar ///////////

////// Start Of Grammar Actions //////

template<typename Rule>
struct action
{};

template<>
struct action<set_cam_pos>
{
    template<typename ParseInput>
    static void apply(const ParseInput& /* in */, Command& cmd)
    {
        using Target = CmdSet::Target;
        cmd = make<CmdSet>(Target::CAMERA_POS);
    }
};

template<>
struct action<set_cam_dir>
{
    template<typename ParseInput>
    static void apply(const ParseInput& /* in */, Command& cmd)
    {
        using Target = CmdSet::Target;
        cmd = make<CmdSet>(Target::CAMERA_DIR);
    }
};

/////// End Of Grammar Actions ///////

template<typename ParseInput>
constexpr Command parse(ParseInput& input)
{
    Command cmd = make<CmdNone>();

    try
    {
        pegtl::parse<grammar, action>(input, cmd);
    }
    catch(const std::exception& e)
    {
        return make<CmdNone>();;
        // std::cerr << e.what() << '\n';
    }

    return cmd;
}

}

#endif // COMMAND_PARSER_H