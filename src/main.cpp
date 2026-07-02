#include <iostream>

#include "math/math_all.h"
#include "renderer/render_types.hpp"
#include "renderer/renderer.h"
#include "renderer/engine.h"
#include "renderer/framebuffer.h"
#include "scene/mesh.h"
#include "scene/camera.h"

RenderState parse_parameters(int argc, char* argv[], std::string& export_filepath)
{
    RenderState render_state;
    for(int i = 2; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "BlinnPhong")
        {
            render_state.shader_type = BLINN_PHONG;
        }
        if (arg == "PBR")
        {
            render_state.shader_type = PBR;
        }
        if (arg == "--line")
        {
            render_state.polygon_mode = LINE;
        }
        if (arg == "--fill")
        {
            render_state.polygon_mode = FILL;
        }
        if (arg == "-e" or arg == "--export")
        {
            if (i + 1 >= argc)
            {
                std::cout << "Default Export filepath: ../../results/output.ppm" << std::endl;
            } else {
                i++;
                std::string filepath = argv[i];
                if (filepath.length() >= 4 && filepath.compare(filepath.length() - 4, 4, ".ppm") == 0) 
                {
                    export_filepath = filepath;
                } else
                {
                    std::cout << arg << " Requires .ppm Format Export Filepath " << std::endl;
                    std::cout << "Default Export Filepath: ../../results/output.ppm" << std::endl;
                }
            }
            // Export filepath Valid Check
            
        }
        // TODO: CullMode, DepthTest...
    }
    return render_state;
}

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << "<scene_name> " << "BlinnPhong/PBR " << "<args>" << std::endl;
        // TODO : list all possible test scenes
        // TODO : --help
        return 0;
    }

    std::string scene_name = argv[1];
    std::string export_filepath = PROJECT_SOURCE_DIR "/results/output.ppm";
    RenderState render_state = parse_parameters(argc, argv, export_filepath);

    Engine engine;
    if(engine.start_up(scene_name, render_state) < 0) return -1;

    bool headless = false, parallel = false, interactive = false;
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--headless")    { headless = true; }
        if (std::string(argv[i]) == "--parallel")    { parallel = true; }
        if (std::string(argv[i]) == "--interactive") { interactive = true; }
    }
    if (headless) {
        if (parallel)
            engine.render_one_frame_parallel();
        else
            engine.render_one_frame();
    } else if (interactive) {
        if (parallel)
            engine.run_interactive_parallel();
        else
            engine.run_interactive();
    } else {
        engine.run();
    }

    engine.export_ppm_image(export_filepath);

    return 0;
}