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

// int main()
// {
//     namespace pegtl = tao::pegtl;

//     pegtl::string_input arg1("set camera.dir", "from_main");

//     kzn::Command cmd = kzn::parse(arg1);

//     switch(cmd.type)
//     {
//         case kzn::CmdType::CMD_SET :
//             switch(cmd.data.cmd_set.target)
//             {
//                 case kzn::CmdSet::Target::CAMERA_POS:
//                     std::cout << "Set(Position)\n";
//                     break;

//                 case kzn::CmdSet::Target::CAMERA_DIR:
//                     std::cout << "Set(Direction)\n";
//                     break;
//             }
//             break;

//         case kzn::CmdType::CMD_NONE:
//             std::cout << "NO COMMAND\n";
//             break;

//         default:
//             break;
//     }

//     return 0;
// }


#include "utils.hpp"

int main()
{
    using namespace kzn::utils;

    Queue<int,5> q;
    q.push(1);
    q.push(2);
    q.push(3);

    std::cout << q.pop() << '\n';
    std::cout << q.pop() << '\n';
    std::cout << q.pop() << '\n';
    // q.push(4);

    return 0;
}