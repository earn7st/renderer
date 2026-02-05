#include "scene/scene.h"

void Scene::add_model(const Model& model)
{
    models_.push_back(model);
}

void Scene::add_light(const DirectionalLight& light)
{
    lights_.push_back(std::make_shared<DirectionalLight>(light));
}

const Camera& Scene::get_main_camera() const
{
    return main_camera_;
}
const std::vector<Model>& Scene::get_models() const
{
    return models_;
}

void Scene::set_main_camera(const Camera& camera) 
{
    main_camera_ = camera;
}

void Scene::print_info() const
{
    for (size_t i = 0; i < models_.size(); ++i)
    {
        const Model& model = models_[i];
        model.print_info();
    }
    
    for (size_t i = 0; i < lights_.size(); ++i)
    {
        const auto spLight = lights_[i];
        spLight->print_info();
    }

    main_camera_.print_info();

    return;
}

