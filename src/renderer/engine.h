#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <chrono>

#include "renderer/input_handler.h"
#include "renderer/displayer.h"
#include "renderer/resource_manager.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "scene/fps_camera.h"

class Engine
{
public:
    Engine(uint32_t w = DEFAULT_DISPLAYER_WIDTH, uint32_t h = DEFAULT_DISPLAYER_HEIGHT);
    int start_up(const std::string& scene_name, const RenderState& render_state);
    void run();
    void run_interactive();          // FPS camera + single-threaded per-frame
    void run_interactive_parallel(); // FPS camera + tile-parallel per-frame
    void render_one_frame();
    void render_one_frame_parallel();
    void export_ppm_image(const std::string& export_filepath);
    int shut_down();

    void print_info() const;

    // Expose camera for external configuration
    FpsCamera& camera() { return fps_camera_; }

private:
    InputHandler    input_handler_;
    Displayer       displayer_;
    ResourceManager resource_manager_;
    Scene           scene_;
    Renderer        renderer_;
    FpsCamera       fps_camera_;

    Framebuffer main_framebuffer_;
};

#endif
