#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "scene/material.h"
#include "scene/mesh.h"
#include "scene/texture.h"

#include <vector>
#include <memory>
#include <unordered_map>

class ResourceManager
{
public:
    ResourceManager() = default;
    
    void init_shaders();

    const std::shared_ptr<Shader>& register_shader(const std::string&, const std::shared_ptr<Shader>);
    const std::shared_ptr<Mesh>& load_mesh(const std::shared_ptr<Mesh>);
    const std::shared_ptr<Material>& load_material(const std::string&, std::shared_ptr<Material>);
    const std::shared_ptr<Texture>& load_texture(const std::string&, std::shared_ptr<Texture>);

    const std::string& get_scene_assets_root_path() const { return scene_assets_root_path_; }
    const std::shared_ptr<Shader>& get_shader(const std::string& name) { return shaders_[name]; }
    const std::vector<std::shared_ptr<Mesh>>& get_meshes() { return meshes_; }
    const std::shared_ptr<Material>& get_material(const std::string& name) { return materials_[name]; }
    const std::shared_ptr<Texture>& get_texture(const std::string& name) { return textures_[name]; }

    void set_scene_assets_root_path(const std::string& s) { scene_assets_root_path_ = s; }

    void print_resources() const;

private:
    std::string scene_assets_root_path_;

    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;
    std::vector<std::shared_ptr<Mesh>> meshes_; 
    std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;
    
};

#endif