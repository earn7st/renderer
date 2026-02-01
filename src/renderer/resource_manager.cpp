#include "renderer/resource_manager.h"
#include "renderer/shader.h"

void ResourceManager::init_shaders()
{
    register_shader("blinn_phong", std::make_shared<Shader>(standard_vertex_shader, blinn_phong_fragment_shader));
    register_shader("wireframe", std::make_shared<Shader>(standard_vertex_shader, wireframe_fragment_shader));
    register_shader("flat", std::make_shared<Shader>(standard_vertex_shader, flat_fragment_shader));
}

const std::shared_ptr<Shader>& ResourceManager::register_shader(const std::string& name, const std::shared_ptr<Shader> shader)
{
    shaders_.insert({name, shader});
    return shaders_[name];
}

const std::shared_ptr<Mesh>& ResourceManager::load_mesh(const std::shared_ptr<Mesh> mesh)
{
    meshes_.push_back(mesh);
    return meshes_.back();
}

const std::shared_ptr<Material>& ResourceManager::load_material(const std::string& name, const std::shared_ptr<Material> material)
{
    materials_.insert({name, material});
    return materials_[name];
}

const std::shared_ptr<Texture>& ResourceManager::load_texture(const std::string& name, const std::shared_ptr<Texture> texture)
{
    textures_.insert({name, texture});
    return textures_[name];
}

