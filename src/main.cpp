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
    struct set
        : pegtl::string<'s','e','t'>
    {};

    struct name
       : pegtl::plus<pegtl::alpha>
    {};

    struct camera_pos
        : pegtl::string<'c','a','m','e','r','a','.','p','o','s'>
    {};

    struct camera_pos
        : pegtl::string<'c','a','m','e','r','a','.','p','o','s'>
    {};

    struct camera_dir
        : pegtl::string<'c','a','m','e','r','a','.','d','i','r'>
    {};

    struct set_cmd
        : pegtl::seq<set, pegtl::sor<camera_pos, camera_dir>>
    {}

    struct grammar
       : pegtl::must<set, pegtl::plus<pegtl::one<' '>>, name, pegtl::eof>
    {};

    template<typename Rule>
    struct action
    {};

    template<>
    struct action<name>
    {
        template<typename ParseInput>
        static void apply(const ParseInput& in, kzn::Command& cmd)
        {
            using Target = kzn::CmdSet::Target;
            cmd = kzn::make<kzn::CmdSet>(Target::CAMERA_POS)
        }
    };

}  // namespace cmd_parser

const kzn::Command parse(const std::string& input)
{
    kzn::Command cmd{kzn::CmdType::CMD_NONE};

    pegtl::parse<cmd_parser::grammar, cmd_parser::action>(in, cmd);

    return cmd;
}

int main( int argc, char* argv[] )
{
    if(argc != 2)
        return 1;

    // Start a parsing run of argv[1] with the string
    // variable 'name' as additional argument to the
    // action; then print what the action put there.

    std::string name;

    pegtl::argv_input in(argv, 1);
    
    if(pegtl::parse<cmd_parser::grammar, cmd_parser::action>(in, name))
    {
        std::cout << "Parser returned true\n";

        // using Target = kzn::CmdSet::Target;
        // kzn::Command cmd = kzn::make<kzn::CmdSet>(Target::CAMERA_POS);
    }
    else
    {
        std::cout << "Parser returned false\n";
    }

   std::cout << "Good bye, " << name << "!" << std::endl;
   return 0;
}