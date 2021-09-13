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

int main( int argc, char* argv[] )
{
    if(argc != 2)
        return 1;

    namespace pegtl = tao::pegtl;
    using namespace kzn;

    pegtl::argv_input arg1(argv, 1);

    Command cmd = make<CmdNone>();

    try
    {
        pegtl::parse<grammar, action>(arg1, cmd);
    }
    catch(const std::exception& e)
    {
        // std::cerr << e.what() << '\n';
    }

    switch(cmd.type)
    {
        case kzn::CmdType::CMD_SET :
            switch(cmd.data.cmd_set.target)
            {
            case CmdSet::Target::CAMERA_POS:
                std::cout << "Target: Position\n";
                break;

            case CmdSet::Target::CAMERA_DIR:
                std::cout << "Target: Direction\n";
                break;
            }
            break;

        case kzn::CmdType::CMD_NONE :
            std::cout << "NO COMMAND\n";
            break;

        default:
            break;
    }

    return 0;
}