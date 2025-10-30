#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "scene/material.h"
#include "scene/mesh.h"
#include "scene/texture.h"

class ResourceManager
{
public:
    ResourceManager() = default;

    bool initialize();

    void load_mesh(const Mesh& mesh);
    void load_material(const Material& material);

    const Mesh& get_mesh(int i);
    const Mesh* get_pMesh(int i);

    const std::vector<Mesh>& get_meshes() { return meshes_; }

private:
    std::vector<Mesh> meshes_;
    std::vector<Material> materials_;
    std::vector<Texture> textures_;
};

#endif