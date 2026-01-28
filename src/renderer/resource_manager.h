#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "scene/material.h"
#include "scene/mesh.h"
#include "scene/texture.h"

#ifdef _WIN32
    #include <vector>
    #include <memory>
#endif

class ResourceManager
{
public:
    ResourceManager() = default;
    
    void init_shaders();
    void register_shader(const std::string&, const Shader&);
    
    void load_mesh(const std::shared_ptr<Mesh>);
    void load_material(const std::shared_ptr<Material>);

    const std::vector<std::shared_ptr<Mesh>>& get_spMeshes() { return spMeshes_; }

private:
    std::vector<std::shared_ptr<Mesh>> spMeshes_;
    std::vector<std::shared_ptr<Material>> spMaterials_;
    std::vector<std::shared_ptr<Texture>> spTextures_;
    
    std::unordered_map<std::string, Shader> shader_pool_;
};

#endif