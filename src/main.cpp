#include "kazan.hpp"
// #include "commands.hpp"
// #include "cmd_parser.hpp"
// #include "cmd_input.hpp"

// #include "pegtl.hpp"

#include <iostream>

// #include "utils.hpp"


class Foo
{
    public:
    Foo(int foo)
        : foo(foo) {}

    private:
    int foo;
};

int main()
{
    kzn::TestApp app;

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

///////////////////////////////////////////////

// void print_cmd(const kzn::Command& cmd)
// {
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
//             std::cout << "None\n";
//             break;

//         default:
//             break;
//     }
// }



// int main()
// {
//     std::ios_base::sync_with_stdio(false);
//     auto q = std::make_shared<kzn::CommandInputThread::CmdQueue>();
//     kzn::CommandInputThread t(q);
//     t.start();

//     while(1)
//     {
//         if(!q->is_empty())
//         {
//             // std::cout << "its not empty, why?\n";
//             print_cmd(q->pop());
//             std::cout << "is not empty\n";
//         }
//     }

//     t.join();

//     return 0;
// }

///////////////////////////////////////////////