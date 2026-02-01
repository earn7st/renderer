#ifndef __SCENE_LOADER_H__
#define __SCENE_LOADER_H__

#include "thirdparty/json.hpp"
#include "renderer/resource_manager.h"
#include "scene/model.h"
#include "scene/scene.h"

using json = nlohmann::json;

class TextureLoader
{
public:
    static std::shared_ptr<Texture> load_texture_from_file(const std::string& filepath);
};

class ISceneLoader
{
public:
    virtual ~ISceneLoader() = default;
    virtual int load_scene_from_context_path(const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager) = 0;
};

class JsonSceneLoader : public ISceneLoader
{
public:
    int load_scene_from_context_path(const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager);
    int load_object_from_json(const json& objects_data, const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager);
    
    int load_camera_from_json(const json& data, Scene& scene);
    int load_lights_from_json(const json& data, Scene& scene);

    Model load_model_from_json(const json& data, ResourceManager& r_manager);
    void load_materials_from_json(const json& data, ResourceManager& r_manager);
};


#endif