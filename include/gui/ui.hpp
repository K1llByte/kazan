#ifndef UI_H
#define UI_H

#include "vk/cmd_buffers.hpp"
#include "core/renderer.hpp"
#include "core/texture.hpp"
#include "vk/image.hpp"

namespace kzn {
    class Interface
    {
        private:
        bool             render_ui = false;
        VkDescriptorPool imgui_pool;
        Renderer*        renderer;
        // Temporary:
        Texture   texture;
        vk::Image image;

        public:
        Interface(Renderer* _renderer, Window& _window);
        ~Interface();

        void toggle();

        void setup();
        void draw(vk::CommandBuffer& cmd_buffer);
        void set_theme();
    };
}

#endif // UI_H