#include "renderer/engine.h"
#include "renderer/render_states.h"
#include "renderer/render_types.h"
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
    if(scene_loader->load_scene(scene_context_path, scene_, resource_manager_))
    {
        std::cerr << "Engine::start_up(): Failed to load scene" << std::endl;
        return -1; 
    }

    return 0;
}

void Engine::run()
{
    int frame_cnt = 1;
    // Main Loop!
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