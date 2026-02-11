#include <iostream>

#include "math/math_all.h"
#include "renderer/render_types.hpp"
#include "renderer/renderer.h"
#include "renderer/engine.h"
#include "renderer/framebuffer.h"
#include "scene/mesh.h"
#include "scene/camera.h"

RenderState parse_parameters(int argc, char* argv[])
{
    RenderState render_state;
    for(int i = 2; i < argc; ++i)
    {
        if(argv[i] == "--line")
        {
            render_state.polygon_mode = LINE;
        }
        if(argv[i] == "--fill")
        {
            render_state.polygon_mode = FILL;
        }
        // TODO: CullMode, DepthTest...
    }
    return render_state;
}

int main(int argc, char* argv[])
{
    printf("running\n");
    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << "<scene_name> " << "<args>" << std::endl;
        // TODO : list all possible test scenes
        // TODO : --help
        return 0;
    }

    std::string scene_name = argv[1];
    RenderState render_state = parse_parameters(argc, argv);
    
    Engine engine;
    if(engine.start_up(scene_name, render_state) < 0) return -1;

    //engine.print_info();

    engine.run();

    return 0;
}