#ifndef __SCENE_H__
#define __SCENE_H__

#include "scene/model.h"
#include "scene/material.h"
#include "scene/camera.h"
#include "scene/light.h"
    
class Scene
{
public:
    Scene() = default;

    void add_model(const Model& model);
    void add_light(const DirectionalLight&);

    const Camera& get_main_camera() const;
    const std::vector<Model>& get_models() const;

    void set_main_camera(const Camera& camera);

    void print_info() const;

private:
    std::vector<Model> models_;
    std::vector<std::shared_ptr<Light>> lights_;
    Camera main_camera_;
};

#endif