#pragma once

#include "core/console.hpp"
#include "core/log.hpp"
#include "editor/panel.hpp"
#include "events/event_manager.hpp"
#include "fmt/format.h"
#include "imgui.h"
#include "input/input.hpp"
#include "input/keyboard.hpp"
#include <algorithm>
#include <cstring>
#include <numeric>
#include <ranges>
#include <string_view>

namespace kzn {

class ConsolePanel
    : public Panel
    , public EventListener {
public:
    ConsolePanel(Window& window, Console& console)
        : m_window_ptr{&window}
        , m_console_ptr{&console} {
        listen(&ConsolePanel::on_key_event);
    }

    ~ConsolePanel() = default;

    void update(float delta_time) override {
        if (m_enabled) {
            ImGui::PushStyleColor(
                ImGuiCol_WindowBg, ImVec4(0.001f, 0.001f, 0.001f, 0.46f)
            );
            ImGui::PushStyleColor(
                ImGuiCol_FrameBg, ImVec4(0.00f, 0.00f, 0.00f, 0.99f)
            );
            ImGui::PushStyleVar(
                ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)
            );
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::SetNextWindowSize(ImVec2(
                m_window_ptr->extent().width - 40,
                m_window_ptr->extent().height - 40
            ));
            ImGui::SetNextWindowPos(ImVec2(20, 20));

            constexpr auto console_win_flags =
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoScrollbar;
            ImGui::Begin("Console", nullptr, console_win_flags);

            // =========== Log child window =========== //

            // Reserve enough left-over height for 1 separator + 1 input text
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
            const auto region_size =
                ImVec2(0, -ImGui::GetFrameHeightWithSpacing());
            constexpr auto region_child_flags =
                ImGuiChildFlags_NavFlattened |
                ImGuiChildFlags_AlwaysUseWindowPadding;
            constexpr auto region_win_flags =
                ImGuiWindowFlags_HorizontalScrollbar |
                ImGuiWindowFlags_NoScrollbar;
            if (ImGui::BeginChild(
                    "ScrollingRegion",
                    region_size,
                    region_child_flags,
                    region_win_flags
                )) {

                if (ImGui::BeginPopupContextWindow()) {
                    if (ImGui::Selectable("Clear")) {
                        m_logs.clear();
                    }
                    ImGui::EndPopup();
                }

                for (const auto& log : m_logs) {
                    // If is error
                    if (log.first) {
                        ImGui::PushStyleColor(
                            ImGuiCol_Text, IM_COL32(0xC2, 0x1E, 0x1E, 0xFF)
                        );
                        ImGui::TextUnformatted(log.second.c_str());
                        ImGui::PopStyleColor();
                    }
                    else {
                        ImGui::TextUnformatted(log.second.c_str());
                    }
                }

                if (m_scroll_down) {
                    ImGui::SetScrollHereY(1.0f);
                    m_scroll_down = false;
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();

            // =========== Input bar =========== //

            ImGui::PushItemWidth(-1);
            bool reclaim_focus = false;
            const auto input_flags = ImGuiInputTextFlags_EnterReturnsTrue |
                                     ImGuiInputTextFlags_CallbackCompletion |
                                     ImGuiInputTextFlags_CallbackHistory;
            ImGui::SetKeyboardFocusHere();
            if (ImGui::InputText(
                    "Input",
                    m_console_input,
                    IM_ARRAYSIZE(m_console_input),
                    input_flags,
                    &ConsolePanel::input_text_callback,
                    static_cast<void*>(this)
                )) {
                char* input_str = m_console_input;
                // Update history state
                m_history_pos = -1;
                m_history.push_back(input_str);

                if (input_str[0]) {
                    bool executed = m_console_ptr->execute_cmd(
                        std::string_view{input_str, std::strlen(input_str) + 1}
                    );

                    if (executed) {
                        m_logs.emplace_back(false, input_str);
                    }
                    else {
                        m_logs.emplace_back(
                            true,
                            fmt::format("Command not found '{}'", input_str)
                        );
                    }
                    m_scroll_down = true;
                }
                std::strcpy(input_str, "");
                reclaim_focus = true;
            }
            ImGui::SetItemDefaultFocus();

            if (reclaim_focus) {
                // Auto focus previous widget
                ImGui::SetKeyboardFocusHere(-1);
            }

            ImGui::PopItemWidth();
            ImGui::End();
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(2);
        }
    }

    static int input_text_callback(ImGuiInputTextCallbackData* data) {
        auto console_panel_ptr = static_cast<ConsolePanel*>(data->UserData);
        switch (data->EventFlag) {
            case ImGuiInputTextFlags_CallbackCompletion: {
                return console_panel_ptr->input_text_completion_callback(data);
            } break;
            case ImGuiInputTextFlags_CallbackHistory: {
                return console_panel_ptr->input_text_history_callback(data);
            } break;
        }

        return 0;
    }

    int input_text_completion_callback(ImGuiInputTextCallbackData* data) {
        const auto commands = m_console_ptr->cmds();

        // Locate beginning of current word
        const char* word_end = data->Buf + data->CursorPos;
        const char* word_start = word_end;
        while (word_start > data->Buf) {
            const char c = word_start[-1];
            if (c == ' ' || c == '\t' || c == ',' || c == ';')
                break;
            word_start--;
        }

        // Build a list of candidates
        std::vector<std::string_view> candidates;
        for (const auto& command : commands) {
            if (std::ranges::equal(
                    command | std::ranges::views::take(word_end - word_start),
                    std::string_view{word_start, size_t(word_end - word_start)}
                )) {
                candidates.push_back(command);
            }
        }

        if (candidates.size() == 1) {
            // Single match. Delete the beginning of the word and replace it
            // entirely so we've got nice casing.
            data->DeleteChars(
                (int)(word_start - data->Buf), (int)(word_end - word_start)
            );
            data->InsertChars(data->CursorPos, candidates[0].data());
            // data->InsertChars(data->CursorPos, " ");
            return 0;
        }
        else if (candidates.size() > 1) {
            // Multiple matches. Complete as much as we can..
            // So inputting "C"+Tab will complete to "CL" then display "CLEAR
            // and "CLASSIFY" as matches.
            int match_len = int(word_end - word_start);
            for (;;) {
                int c = 0;
                bool all_candidates_matches = true;
                for (int i = 0; i < candidates.size() && all_candidates_matches;
                     i++)
                    if (i == 0)
                        c = toupper(candidates[i][match_len]);
                    else if (c == 0 || c != toupper(candidates[i][match_len]))
                        all_candidates_matches = false;
                if (!all_candidates_matches)
                    break;
                match_len++;
            }

            if (match_len > 0) {
                data->DeleteChars(
                    (int)(word_start - data->Buf), (int)(word_end - word_start)
                );
                data->InsertChars(
                    data->CursorPos,
                    candidates[0].data(),
                    candidates[0].data() + match_len
                );
            }

            // List matches
            m_logs.emplace_back(false, "Commands:");
            for (const auto& candidate : candidates) {
                m_logs.emplace_back(false, fmt::format("- {}", candidate));
            }
        }
        return 0;
    }

    int input_text_history_callback(ImGuiInputTextCallbackData* data) {
        // Based on imgui_demo implementation
        const int prev_history_pos = m_history_pos;
        if (data->EventKey == ImGuiKey_UpArrow) {
            if (m_history_pos == -1)
                m_history_pos = m_history.size() - 1;
            else if (m_history_pos > 0)
                m_history_pos--;
        }
        else if (data->EventKey == ImGuiKey_DownArrow) {
            if (m_history_pos != -1)
                if (++m_history_pos >= m_history.size())
                    m_history_pos = -1;
        }

        // A better implementation would preserve the data on the current input
        // line along with cursor position.
        if (prev_history_pos != m_history_pos) {
            const std::string& history_str =
                (m_history_pos >= 0) ? m_history[m_history_pos] : "";
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, history_str.data());
        }
        return 0;
    }

    void on_key_event(const KeyboardKeyEvent& event) {
        if (event.key == KeyboardKey::F1 &&
            event.action == InputAction::Release) {
            set_enabled(!m_enabled);
        }
    }

    void set_theme() {
        auto colors_ptr = ImGui::GetStyle().Colors;

        colors_ptr[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.96f);
        colors_ptr[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
    }

private:
    Window* m_window_ptr;
    Console* m_console_ptr;
    std::vector<std::pair<bool, std::string>> m_logs;
    char m_console_input[256];
    bool m_scroll_down = false;
    // History callback state
    int m_history_pos = -1;
    std::vector<std::string> m_history;
};

} // namespace kzn