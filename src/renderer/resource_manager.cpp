#include "renderer/resource_manager.h"
#include "renderer/shader.h"

void ResourceManager::init_shaders()
{
    register_shader("blinn_phong", Shader(standard_vertex_shader, blinn_phong_fragment_shader));
    register_shader("wireframe", Shader(standard_vertex_shader, wireframe_fragment_shader));
    register_shader("flat", Shader(standard_vertex_shader, flat_fragment_shader));
}

void ResourceManager::register_shader(const std::string& name, const Shader& shader)
{
    shader_pool_[name] = shader;
}

void ResourceManager::load_mesh(const std::shared_ptr<Mesh> mesh)
{
    spMeshes_.push_back(mesh);
}

void ResourceManager::load_material(const std::shared_ptr<Material> material)
{
    spMaterials_.push_back(material);
}
