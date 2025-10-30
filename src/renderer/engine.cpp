#include "renderer/engine.h"

Engine::Engine(uint32_t w, uint32_t h) 
: displayer_(), input_handler_(), resource_manager_(), scene_(), renderer_(), main_framebuffer_(w, h){}

int Engine::start_up(const std::string& scene_name)
{   
    =
    return 0;
}

/*
int Engine::start_up(const std::string& scene_name)
{   
    // 1. 确定当前场景的目录 (Scene Context Root)
    std::string scene_context_root = SCENE_ROOT_DIR + scene_name + "/";
    
    // 2. 构造主配置文件的完整路径
    std::string config_filepath = scene_context_root + "scene.json";
    
    // 3. 验证文件是否存在... (略)
    
    // 4. 引入场景加载器
    std::unique_ptr<ISceneLoader> scene_loader = std::make_unique<JsonSceneLoader>();

    // 关键点：将场景的上下文根目录 scene_context_root 传递给加载器
    if (!scene_loader->load_scene(config_filepath, scene_, resource_manager_, scene_context_root))
    {
        std::cerr << "Engine::start_up(): Failed to load scene." << std::endl;
        return -1;
    }

    return 0;
}
*/

void Engine::run()
{
    int frame_cnt = 1;
    // Main Loop!
    bool running = true;

    while(running)
    {
        // Handle Input
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