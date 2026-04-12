#include "renderer/engine.h"
#include "renderer/render_states.h"
#include "renderer/render_types.hpp"
#include "scene/scene_loader.h"

#define SCENE_PATH  "../scenes/"

Engine::Engine(uint32_t w, uint32_t h)  
: displayer_(w, h), input_handler_(), resource_manager_(), scene_(), renderer_(), main_framebuffer_(w, h){}

int Engine::start_up(const std::string& scene_name, const RenderState& render_state)
{   
    // 1. Attach Framebuffer
    if (!renderer_.attach_framebuffer(&main_framebuffer_)) { std::cerr << "Engint::start_up(): Failed to Attach Framebuffer" << std::endl; }

    // 2. Set Render States
    renderer_.set_render_state(render_state);
    
    // 3. Shader Registration
    resource_manager_.init_shaders();

    // 4. Load Scene
    std::string scene_context_path = SCENE_PATH + scene_name + "/";
    std::unique_ptr<ISceneLoader> scene_loader = std::make_unique<JsonSceneLoader>();
    if(scene_loader->load_scene_from_context_path(scene_context_path, scene_, resource_manager_, render_state.shader_type))
    {
        std::cerr << "Engine::start_up(): Failed to load scene" << std::endl;
        return -1; 
    }

    // 5. Initialize Lights
    renderer_.initialize_lights(scene_);

    return 0;
}

void Engine::run()
{
    int frame_cnt = 1;
    bool running = true;

    while(running)
    {
        while(input_handler_.poll_event())
        {
            if(!input_handler_.handle_event())
            {
                running = false;
            }
        }

        if(frame_cnt > 0) 
        {
            frame_cnt--;
            main_framebuffer_.clear();
            
            renderer_.render(scene_);
            
        }
        displayer_.present(main_framebuffer_);
    }

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