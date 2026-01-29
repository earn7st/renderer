#ifndef __SCENE_LOADER_H__
#define __SCENE_LOADER_H__

#include "json.hpp"
#include "renderer/resource_manager.h"
#include "scene/model.h"
#include "scene/scene.h"

using json = nlohmann::json;

class ISceneLoader
{
public:
    virtual ~ISceneLoader() = default;
    virtual int load_scene(const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager) = 0;
};

class JsonSceneLoader : public ISceneLoader
{
public:
    int load_scene(const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager);
    int load_object(const json& objects_data, const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager);
    
    int load_camera(const json& data, Scene& scene);
    int load_lights(const json& data, Scene& scene);

    Model load_model(const json& data, ResourceManager& r_manager);
    void load_materials(const json& data, ResourceManager& r_manager);
};


#endif