#include "test_app.hpp"
#include "commands.hpp"
#include "cmd_parser.hpp"

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

int main()
{
    namespace pegtl = tao::pegtl;

    // pegtl::argv_input arg1(argv, 1);
    pegtl::string_input arg1( "set camera.dir", "from_main" );

    kzn::Command cmd = kzn::parse(arg1);

    switch(cmd.type)
    {
        case kzn::CmdType::CMD_SET :
            switch(cmd.data.cmd_set.target)
            {
            case kzn::CmdSet::Target::CAMERA_POS:
                std::cout << "Target: Position\n";
                break;

            case kzn::CmdSet::Target::CAMERA_DIR:
                std::cout << "Target: Direction\n";
                break;
            }
            break;

        case kzn::CmdType::CMD_NONE:
            std::cout << "NO COMMAND\n";
            break;

        default:
            break;
    }

    return 0;
}