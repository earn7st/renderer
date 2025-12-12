#include "renderer/resource_manager.h"

void ResourceManager::load_mesh(const std::shared_ptr<Mesh> mesh)
{
    spMeshes_.push_back(mesh);
}

void ResourceManager::load_material(const std::shared_ptr<Material> material)
{
    spMaterials_.push_back(material);
}
