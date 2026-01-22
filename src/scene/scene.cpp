#include "scene/scene.h"

void Scene::add_model(const Model& model)
{
    models_.push_back(model);
}

void Scene::add_light(const DirectionalLight dl)
{
    lights_.push_back(dl);
}

void Scene::add_light(const PointLight pl)
{
    lights_.push_back(pl);
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

