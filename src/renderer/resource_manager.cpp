#include "renderer/resource_manager.h"
#include "renderer/shader.h"

void ResourceManager::init_shaders()
{
    register_shader("blinn_phong", std::make_shared<Shader>("blinn_phong", standard_vertex_shader, blinn_phong_fragment_shader));
    register_shader("PBR", std::make_shared<Shader>("PBR", standard_vertex_shader, PBR_fragment_shader));
    register_shader("wireframe", std::make_shared<Shader>("wireframe", standard_vertex_shader, wireframe_fragment_shader));
    register_shader("flat", std::make_shared<Shader>("flat", standard_vertex_shader, flat_fragment_shader));
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

void ResourceManager::print_resources() const
{
    std::cout << "==== ResourceManager Content ====\n";
    std::cout << "---- Shaders ----\n";
    for (const auto& [key, shader]: shaders_)
    {
        std::cout << "Key: " << key << " " << "Name: " << shader->get_name() << std::endl;
    }
    std::cout << "---- Meshes ----\n";
    for (size_t i = 0; i < meshes_.size(); ++i)
    {
        const auto& mesh = meshes_[i];
        if (mesh)
        {
            std::cout << "Mesh No." << i << ": ";
            std::cout << "Num_Vertices " << mesh->num_vertices << ", Num_Faces " << mesh->num_faces << std::endl;
        }
    }
    std::cout << "---- Materials ----\n";
    for (const auto& [key, material] : materials_)
    {
        std::cout << "Key: " << key << std::endl;
        material->print_info();
        std::cout << std::endl;
    }

    std::cout << "---- Textures ----\n";
    for (const auto& [key, texture] : textures_)
    {
        std::cout << "Key: " << key << std::endl;
        texture->print_info();
        std::cout << std::endl;
    }
}

