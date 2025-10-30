#ifndef __SCENE_H__
#define __SCENE_H__

#include "scene/model.h"
#include "scene/material.h"
#include "scene/camera.h"
    
class Scene
{
public:
    Scene() = default;

    bool initialize();
    void add_model(const Model& model);

    const Camera& get_main_camera() const;
    const std::vector<Model>& get_models() const;

private:
    std::vector<Model> models_;
    Camera main_camera_;
};

#endif