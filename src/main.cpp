#include <iostream>

#include "math/math_all.h"
#include "renderer/render_types.h"
#include "renderer/renderer.h"
#include "renderer/engine.h"
#include "renderer/framebuffer.h"
#include "scene/mesh.h"
#include "scene/camera.h"


#define SCENE_PATH  "../scenes/"

int main(int argc, char* argv[])
{

    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << "<scene_name>" << std::endl;
        // TODO : list all possible test scenes
        return 0;
    }

    std::string scene_name = argv[1];
    
    Engine engine;
    if(engine.start_up(scene_name) < 0) return -1;
    engine.run();

    return 0;
}