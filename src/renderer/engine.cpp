#include <fstream>
#include <chrono>

#include "renderer/engine.h"
#include "renderer/render_states.h"
#include "renderer/render_types.hpp"
#include "scene/scene_loader.h"

#define SCENE_PATH  PROJECT_SOURCE_DIR "/scenes/"

Engine::Engine(uint32_t w, uint32_t h)
    : displayer_(w, h), input_handler_(), resource_manager_(),
      scene_(), renderer_(), main_framebuffer_(w, h) {}

int Engine::start_up(const std::string& scene_name, const RenderState& render_state)
{
    // 1. Attach Framebuffer
    if (!renderer_.attach_framebuffer(&main_framebuffer_)) {
        std::cerr << "Engine::start_up(): Failed to Attach Framebuffer" << std::endl;
    }

    // 2. Set Render States
    renderer_.set_render_state(render_state);

    // 3. Shader Registration
    resource_manager_.init_shaders();

    // 4. Load Scene
    std::string scene_context_path = SCENE_PATH + scene_name + "/";
    std::unique_ptr<ISceneLoader> scene_loader = std::make_unique<JsonSceneLoader>();
    if (scene_loader->load_scene_from_context_path(scene_context_path, scene_, resource_manager_, render_state.shader_type))
    {
        std::cerr << "Engine::start_up(): Failed to load scene" << std::endl;
        return -1;
    }

    // 5. Initialize FPS camera from the scene's main camera
    fps_camera_.init_from_camera(scene_.get_main_camera());

    // 6. Initialize Lights
    renderer_.initialize_lights(scene_);

    return 0;
}

void Engine::run()
{
    int  frame_limit = 1;
    bool running     = true;
    auto last_time   = std::chrono::high_resolution_clock::now();
    int  frames_done = 0;

    while (running)
    {
        while (input_handler_.poll_event())
        {
            if (!input_handler_.handle_event())
                running = false;
        }

        if (frame_limit > 0)
        {
            frame_limit--;
            main_framebuffer_.clear();
            renderer_.render(scene_, fps_camera_);
            frames_done++;
        }
        displayer_.present(main_framebuffer_);

        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - last_time).count();
        if (dt >= 0.5f && frames_done > 0) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Renderer | FPS: %.1f | frame time: %.1f ms",
                     1.0f / dt, dt * 1000.0f);
            displayer_.set_title(buf);
        }
    }
}

void Engine::run_interactive()
{
    bool running = true;
    auto last_time  = std::chrono::high_resolution_clock::now();
#ifdef NDEBUG
    auto fps_timer  = last_time;
#endif
    int  frame_cnt  = 0;

    // Grab mouse for FPS look
    input_handler_.set_window(displayer_.get_window());
    input_handler_.set_mouse_grabbed(true, displayer_.get_window());

    while (running)
    {
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - last_time).count();
        last_time = now;

        InputState input;
        float mouse_dx, mouse_dy;
        if (!input_handler_.poll_frame_input(input, mouse_dx, mouse_dy))
            running = false;

        fps_camera_.update(input, dt);

        main_framebuffer_.clear();
        renderer_.render(scene_, fps_camera_);
#ifndef NDEBUG
        auto t3 = std::chrono::high_resolution_clock::now();
        float rnd_ms = std::chrono::duration<float, std::milli>(t3 - now).count();
#endif

        displayer_.present(main_framebuffer_);
#ifndef NDEBUG
        auto t4 = std::chrono::high_resolution_clock::now();
        float prs_ms = std::chrono::duration<float, std::milli>(t4 - t3).count();
        char diag[256];
        snprintf(diag, sizeof(diag), "rnd:%.0f prs:%.0f | %.0f fps",
                 rnd_ms, prs_ms, 1000.f / std::max(1.f, rnd_ms + prs_ms));
        displayer_.set_title(diag);
#endif

        frame_cnt++;
#ifdef NDEBUG
        auto elapsed = std::chrono::duration<float>(now - fps_timer).count();
        if (elapsed >= 0.5f) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Renderer | FPS: %d | dt: %.1f ms",
                     (int)(frame_cnt / elapsed), dt * 1000.0f);
            displayer_.set_title(buf);
            frame_cnt = 0;
            fps_timer = now;
        }
#endif
    }

    input_handler_.set_mouse_grabbed(false, displayer_.get_window());
}

void Engine::run_interactive_parallel()
{
    bool running = true;
    auto last_time  = std::chrono::high_resolution_clock::now();
#ifdef NDEBUG
    auto fps_timer  = last_time;
#endif
    int  frame_cnt  = 0;

    input_handler_.set_window(displayer_.get_window());
    input_handler_.set_mouse_grabbed(true, displayer_.get_window());

    while (running)
    {
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - last_time).count();
        last_time = now;

        InputState input;
        float mouse_dx, mouse_dy;
        if (!input_handler_.poll_frame_input(input, mouse_dx, mouse_dy))
            running = false;

        fps_camera_.update(input, dt);

        main_framebuffer_.clear();
        renderer_.render_parallel(scene_, fps_camera_);
#ifndef NDEBUG
        auto t3 = std::chrono::high_resolution_clock::now();
        auto t0 = now;
        float rnd_ms = std::chrono::duration<float, std::milli>(t3 - t0).count();
#endif

        displayer_.present(main_framebuffer_);
#ifndef NDEBUG
        auto t4 = std::chrono::high_resolution_clock::now();
        float prs_ms = std::chrono::duration<float, std::milli>(t4 - t3).count();
        char diag[256];
        snprintf(diag, sizeof(diag), "rnd:%.0f prs:%.0f | %.0f fps [MT]",
                 rnd_ms, prs_ms, 1000.f / std::max(1.f, rnd_ms + prs_ms));
        displayer_.set_title(diag);
#endif

        frame_cnt++;
#ifdef NDEBUG
        auto elapsed = std::chrono::duration<float>(now - fps_timer).count();
        if (elapsed >= 0.5f) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Renderer | FPS: %d | dt: %.1f ms [MT]",
                     (int)(frame_cnt / elapsed), dt * 1000.0f);
            displayer_.set_title(buf);
            frame_cnt = 0;
            fps_timer = now;
        }
#endif
    }

    input_handler_.set_mouse_grabbed(false, displayer_.get_window());
}

void Engine::render_one_frame()
{
    main_framebuffer_.clear();
    renderer_.render(scene_, fps_camera_);
}

void Engine::render_one_frame_parallel()
{
    main_framebuffer_.clear();
    renderer_.render_parallel(scene_, fps_camera_);
}

void Engine::export_ppm_image(const std::string& export_filepath)
{
    std::ofstream ofs(export_filepath);

    uint32_t width = main_framebuffer_.get_width();
    uint32_t height = main_framebuffer_.get_height();

    ofs << "P3\n" << width << " " << height << "\n255\n";

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            RGBA color = main_framebuffer_.get_color(x, y);

            int r = static_cast<int>(255.999f * color.x_);
            int g = static_cast<int>(255.999f * color.y_);
            int b = static_cast<int>(255.999f * color.z_);

            ofs << r << " " << g << " " << b << " ";
        }
        ofs << "\n";
    }
    ofs.close();
}

int Engine::shut_down()
{
    return 0;
}

void Engine::print_info() const
{
    const ResourceManager& r_manager = resource_manager_;
    r_manager.print_resources();

    const Scene& scene = scene_;
    scene.print_info();

    return;
}
