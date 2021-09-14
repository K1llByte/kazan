#ifndef COMMAND_INPUT_H
#define COMMAND_INPUT_H

#include "utils.hpp"
#include "cmd_parser.hpp"

#include <thread>
#include <memory>

namespace kzn
{

class CommandInputThread
{
public:

    using CmdQueue = kzn::utils::Queue<kzn::Command, 5>;
    using CmdQueuePtr = std::shared_ptr<CmdQueue>;
    
    CmdQueuePtr queue_p;

private:

    std::thread main;

public:

    CommandInputThread(CmdQueuePtr q);
    ~CommandInputThread();

    void start();

    void join();
};

}

#endif // COMMAND_INPUT_H
