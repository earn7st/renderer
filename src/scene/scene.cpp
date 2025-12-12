#include "scene/scene.h"

void Scene::add_model(const Model& model)
{
    models_.push_back(model);
}

const Camera& Scene::get_main_camera() const
{
    return main_camera_;
}
const std::vector<Model>& Scene::get_models() const
{
    return models_;
}

void Scene::set_main_camera(Camera& camera) 
{
    main_camera_ = camera;
}

