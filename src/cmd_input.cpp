#include "cmd_input.hpp"

#include <iostream>

bool getline_async(std::istream& is, std::string& str, char delim = '\n')
{
    static std::string lineSoFar;
    char inChar;
    int charsRead = 0;
    bool lineRead = false;
    str = "";

    do {
        // std::cout << "in loop\n";
        charsRead = is.readsome(&inChar, 1);
        if (charsRead == 1)
        {
            if (inChar == delim)
            {
                str = lineSoFar;
                lineSoFar = "";
                lineRead = true;
            }
            else
            {
                lineSoFar.append(1, inChar);
            }
        }
    } while (charsRead != 0 && !lineRead);

    return lineRead;
}

namespace kzn
{

CommandInputThread::CommandInputThread(CmdQueuePtr q)
    : queue_p{q} {}


CommandInputThread::~CommandInputThread()
{
    join();
}


void CommandInputThread::start()
{
    main = std::thread([](CmdQueuePtr q)
    {
        namespace pegtl = tao::pegtl;

        std::string input = "";
        while(true)
        {
            std::cout << "before getline\n";
            while(!getline_async(std::cin, input));
            std::cout << "after getline\n";

            // std::getline(std::cin, input);
            // std::cin >> input;

            pegtl::string_input string_input(input, "stdin");
            kzn::Command cmd = kzn::parse(string_input);

            // print(cmd);
            if(cmd.type != kzn::CmdType::CMD_NONE)
            {
                std::cout << "Inserted command\n";
                q->push(cmd);
            }
        }
    }, queue_p);
}


void CommandInputThread::join()
{
    main.join();
}

}
