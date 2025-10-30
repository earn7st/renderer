#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "renderer/displayer.h"
#include "renderer/input_handler.h"
#include "renderer/resource_manager.h"
#include "renderer/renderer.h"
#include "scene/scene.h"

class Engine
{
public:
    Engine(uint32_t w = DEFAULT_DISPLAYER_WIDTH, uint32_t h = DEFAULT_DISPLAYER_HEIGHT);
    int start_up(const std::string& scene_name);
    void run();
    int shut_down();

private:
    Displayer displayer_;
    InputHandler input_handler_;
    ResourceManager resource_manager_;
    Scene scene_;
    Renderer renderer_;

    Framebuffer main_framebuffer_;
};

#endif