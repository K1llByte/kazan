#include "test_app.hpp"
#include "commands.hpp"

#include "pegtl.hpp"

#include <iostream>

// int main()
// {
//     kzn::TestApp app;

//     try
//     {
//         app.run();
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
//     return EXIT_SUCCESS;
// }

///////////////////////////////////////////////

namespace pegtl = tao::pegtl;

/////////////////////////////////

namespace cmd_parser
{
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
       : pegtl::must<pegtl::sor<set_cam_pos, set_cam_dir>, pegtl::eof>
    {};

    ////////// End Of Grammar //////////

    template<typename Rule>
    struct action
    {};

    template<>
    struct action<set_cam_pos>
    {
        template<typename ParseInput>
        static void apply(const ParseInput& in, kzn::Command& cmd)
        {
            using Target = kzn::CmdSet::Target;
            cmd = kzn::make<kzn::CmdSet>(Target::CAMERA_POS);
        }
    };

    template<>
    struct action<set_cam_dir>
    {
        template<typename ParseInput>
        static void apply(const ParseInput& in, kzn::Command& cmd)
        {
            using Target = kzn::CmdSet::Target;
            cmd = kzn::make<kzn::CmdSet>(Target::CAMERA_DIR);
        }
    };

}  // namespace cmd_parser

// kzn::Command parse(pegtl::argv_input input)
// {
//     using Target = kzn::CmdSet::Target;
//     kzn::Command cmd = kzn::make<kzn::CmdSet>(Target::CAMERA_POS);

//     pegtl::parse<cmd_parser::grammar, cmd_parser::action>(input, cmd);

//     return cmd;
// }

int main( int argc, char* argv[] )
{
    if(argc != 2)
        return 1;

    // Start a parsing run of argv[1] with the string
    // variable 'name' as additional argument to the
    // action; then print what the action put there.

    std::string name;

    pegtl::argv_input arg1(argv, 1);

    using Target = kzn::CmdSet::Target;
    kzn::Command cmd = kzn::make<kzn::CmdSet>(Target::CAMERA_POS);

    pegtl::parse<cmd_parser::grammar, cmd_parser::action>(arg1, cmd);

    switch (cmd.type)
    {
        case kzn::CmdType::CMD_SET :
            std::cout << "Target: " << cmd.data.cmd_set.target << "\n";
            break;

        default:
            break;
    }

    
    // if(pegtl::parse<cmd_parser::grammar, cmd_parser::action>(arg1, name))
//     {
//         std::cout << "Parser returned true\n";

//         // using Target = kzn::CmdSet::Target;
//         // kzn::Command cmd = kzn::make<kzn::CmdSet>(Target::CAMERA_POS);
//     }
//     else
//     {
//         std::cout << "Parser returned false\n";
//     }

//    std::cout << "Good bye, " << name << "!" << std::endl;
   return 0;
}