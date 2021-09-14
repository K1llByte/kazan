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

#include <thread>
#include <memory>

// extern "C"
// {
// #include <unistd.h>
// };


void print(const kzn::Command& cmd)
{
    switch(cmd.type)
    {
        case kzn::CmdType::CMD_SET :
            switch(cmd.data.cmd_set.target)
            {
                case kzn::CmdSet::Target::CAMERA_POS:
                    std::cout << "Set(Position)\n";
                    break;

                case kzn::CmdSet::Target::CAMERA_DIR:
                    std::cout << "Set(Direction)\n";
                    break;
            }
            break;

        case kzn::CmdType::CMD_NONE:
            std::cout << "None\n";
            break;

        default:
            break;
    }
}

class CommandInputThread
{
public:

    using CmdQueue = kzn::utils::Queue<kzn::Command, 5>;
    using CmdQueuePtr = std::shared_ptr<CmdQueue>;
    CmdQueuePtr queue_p;

private:

    std::thread main;

public:

    CommandInputThread(CmdQueuePtr q)
        : queue_p{q} {}

    ~CommandInputThread()
    {
        join();
    }

    void start()
    {
        main = std::thread([](CmdQueuePtr q)
        {
            namespace pegtl = tao::pegtl;

            std::string input;
            while(true)
            {
                std::cout << "getline\n";
                std::getline(std::cin, input);
                // std::cin >> input;

                pegtl::string_input string_input(input, "stdin");
                kzn::Command cmd = kzn::parse(string_input);

                print(cmd);
                if(cmd.type != 0)
                    q->push(cmd);
            }
        }, queue_p);
    }

    void join()
    {
        main.join();
    }

};

int main()
{
    auto q = std::make_shared<CommandInputThread::CmdQueue>();
    CommandInputThread t(q);
    t.start();

    bool torf = false;
    while(1)
    {
        if(!q->is_empty())
        {
            torf = true;
            std::cout << "is not empty\n";
        }
    }

    t.join();

    return 0;
}