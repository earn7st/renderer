#include "renderer/resource_manager.h"

bool ResourceManager::initialize()
{
    return true;
}

void ResourceManager::load_mesh(const Mesh& mesh)
{
    meshes_.push_back(mesh);
}

void ResourceManager::load_material(const Material& material)
{
    materials_.push_back(material);
}

const Mesh& ResourceManager::get_mesh(int i)
{
    assert(i < meshes_.size());
    return meshes_[i];
}

const Mesh* ResourceManager::get_pMesh(int i)
{
    assert(i < meshes_.size());
    return &meshes_[i];
}
