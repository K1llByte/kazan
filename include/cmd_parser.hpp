#ifndef CMD_PARSER_H
#define CMD_PARSER_H

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
using camera_pos = pegtl::string<'c','a','m','e','r','a','.','p','o','s'>;
using camera_dir = pegtl::string<'c','a','m','e','r','a','.','d','i','r'>;

struct set
    : pegtl::string<'s','e','t'>
{};

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
    : pegtl::must<
        pegtl::sor<
            set_cam_pos,
            set_cam_dir
            >, pegtl::eof>
{};

////////// End Of Grammar //////////

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

}

#endif // CMD_PARSER_H
