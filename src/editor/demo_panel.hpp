#pragma once

#include "editor/panel.hpp"
#include "input/input.hpp"

#include <imgui.h>

namespace kzn {

class DemoPanel
    : public Panel
    , public EventListener {
public:
    DemoPanel() { listen(&DemoPanel::on_key_event); }

    void update(float delta_time) override {
        if (m_enabled) {
            ImGui::ShowDemoWindow();
        }
    }

    void on_key_event(const KeyboardKeyEvent& event) {
        if (event.key == KeyboardKey::F2 &&
            event.action == InputAction::Release) {
            set_enabled(!m_enabled);
        }
    }
};

} // namespace kzn