#include "test_app.hpp"
// #include "commands.hpp"
// #include "cmd_parser.hpp"
// #include "cmd_input.hpp"

// #include "pegtl.hpp"

#include <iostream>

// #include "utils.hpp"

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

// int main()
// {
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
//     ImGuiIO& io = ImGui::GetIO();
//     // // (void) io;
//     ImGui_ImplGlfw_InitForVulkan();
//     ImGui::StyleColorsDark();

//     ImGui::Begin("My First Tool", nullptr, ImGuiWindowFlags_MenuBar);
//     if (ImGui::BeginMenuBar())
//     {
//         if (ImGui::BeginMenu("File"))
//         {
//             if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
//             if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
//             if (ImGui::MenuItem("Close", "Ctrl+W"))  { /* Do stuff */ }
//             ImGui::EndMenu();
//         }
//         ImGui::EndMenuBar();
//     }

//     float in_color[4];
//     // Edit a color (stored as ~4 floats)
//     ImGui::ColorEdit4("Color", in_color);

//     // Plot some values
//     const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
//     ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

//     // Display contents in a scrolling region
//     ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
//     ImGui::BeginChild("Scrolling");
//     for (int n = 0; n < 50; n++)
//         ImGui::Text("%04d: Some text", n);
//     ImGui::EndChild();
//     ImGui::End();
// }

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