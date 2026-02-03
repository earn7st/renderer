#include "math/math_all.h"
#include "renderer/render_types.h"
#include "scene/scene_loader.h"

#include <fstream>

#include "thirdparty/json.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"

using json = nlohmann::json;

std::shared_ptr<Texture> TextureLoader::load_texture_from_file(const std::string& filepath)
{
    int w, h, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &w, &h, &channels, 4);
    if (!data)
    {
        throw std::runtime_error("Failed to load image:" + filepath);
    }

    std::filesystem::path p(filepath);
    std::string name = p.filename().string();

    std::shared_ptr<Texture> spTexture = std::make_shared<Texture>(data, w, h, name);
    stbi_image_free(data);

    return spTexture;
}

int JsonSceneLoader::load_scene_from_context_path(const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager)
{
    std::string scene_filepath = scene_context_path + "scene.json";

    std::ifstream f(scene_filepath);
    json data = json::parse(f);

    if (data.contains("camera")) 
    {
        json& camera_data = data.at("camera");
        if (load_camera_from_json(camera_data, scene) != 0)
        {
            std::cerr << "JsonSceneLoader::load_scene : Failed Loading Camera Data\n";
        }   
    }

    if (data.contains("object")) 
    {
        json& object_data = data.at("object");
        if (load_object_from_json(object_data, scene_context_path, scene, r_manager) != 0)
        {
            std::cerr << "JsonSceneLoader::load_objects : Failed Loading Objects Data\n";
            return -1;
        }
    } 
    
    if (data.contains("lights")) 
    {
        json& lights_data = data.at("lights");
        if (load_lights_from_json(lights_data, scene) != 0)
        {
            std::cerr << "JsonSceneLoader::load_lights : Failed Loading Lights Data\n";
            return -1;
        }
    }    

    return 0;
}

int JsonSceneLoader::load_object_from_json(const json& data, const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager)
{
    std::string object_name = data.at("name").get<std::string>();
    std::string object_filename = data.at("filename").get<std::string>();

    std::string materials_filename = object_name + "_materials.json";
    std::string materials_filepath = scene_context_path + materials_filename;
    const std::string& scene_assets_root_path = r_manager.concatenate_to_scene_assets_root(object_name + "/");

    std::ifstream f_material(materials_filepath);
    json materials_data = json::parse(f_material);
    load_materials_from_json(materials_data, r_manager);

    std::string model_filepath = scene_context_path + object_filename;
    std::ifstream f_model(model_filepath);
    json model_data = json::parse(f_model);
    Model model = load_model_from_json(model_data, r_manager);

    if (data.contains("transform"))
    {
        json transform_data = data.at("transform");

        std::vector<float> scale = transform_data.at("scale").get<std::vector<float>>();
        Vec3f scale_v(scale[0], scale[1], scale[2]);

        std::vector<float> translation = transform_data.at("translation").get<std::vector<float>>();
        Vec3f translation_v(translation[0], translation[1], translation[2]);

        std::vector<float> rotation = transform_data.at("rotation").get<std::vector<float>>();
        Vec3f rotation_v(rotation[0], rotation[1], rotation[2]);

        Transform transform(scale_v, translation_v, rotation_v);
        model.set_transform(transform);
    }

    scene.add_model(model);
    
    return 0;
}

int JsonSceneLoader::load_camera_from_json(const json& data, Scene& scene)
{

    std::vector<float> position_arr = data.at("position").get<std::vector<float>>();
    Vec3f position_v(position_arr[0], position_arr[1], position_arr[2]);

    std::vector<float> center_arr = data.at("center").get<std::vector<float>>();
    Vec3f center_v(center_arr[0], center_arr[1], center_arr[2]);

    std::vector<float> world_up_arr = data.at("world_up").get<std::vector<float>>();
    Vec3f world_up_v(world_up_arr[0], world_up_arr[1], world_up_arr[2]);

    float aspect = data.at("aspect").get<float>();
    float fov = data.at("fov").get<float>();
    float n_plane = data.at("n_plane").get<float>();
    float f_plane = data.at("f_plane").get<float>();

    Camera camera = Camera(position_v, center_v, world_up_v, aspect, fov, n_plane, f_plane);
    scene.set_main_camera(camera);

    return 0;
}

int JsonSceneLoader::load_lights_from_json(const json& data, Scene& scene)
{ 
    int num_lights = data.size();
    for (int i = 0; i < num_lights; ++i)
    {
        json light_data = data.at(i);
        std::string type = light_data.at("type");

        std::vector<float> color = light_data.at("color").get<std::vector<float>>();
        Vec3f color_v(color[0], color[1], color[2]);
        float intensity = light_data.at("intensity");

        if (type == "Directional")
        {
            std::vector<float> direction = light_data.at("direction").get<std::vector<float>>();
            Vec3f direction_v(direction[0], direction[1], direction[2]);

            DirectionalLight dl;
            dl.direction = direction_v;
            dl.color = color_v;
            dl.intensity = intensity;
            scene.add_light(dl);

        } else if (type == "Point")
        {
            
        } else if (type == "Spot")
        {

        }
    }
    return 0;
}

Model JsonSceneLoader::load_model_from_json(const json& data, ResourceManager& r_manager)
{
    Model model;

    std::string model_name = data.at("name").get<std::string>();
    const json& vertices_data = data.at("vertices");
    size_t num_vertices = vertices_data.size();
    std::shared_ptr<Mesh> new_mesh_ptr = std::make_shared<Mesh>();

    new_mesh_ptr->num_vertices = uint32_t(num_vertices);
    for (int i = 0; i < num_vertices; ++i)
    {
        json vertex_data = vertices_data.at(i);

        std::vector<float> pos = vertex_data.at("p").get<std::vector<float>>();
        Vec4f pos_v(pos[0], pos[1], pos[2], 1.0f);
        
        std::vector<float> norm = vertex_data.at("n").get<std::vector<float>>();
        Vec4f norm_v(norm[0], norm[1], norm[2], 0.0f);

        std::vector<float> texcoord = vertex_data.at("t").get<std::vector<float>>();
        Vec2f texcoord_v(texcoord[0], texcoord[1]);

        Vertex vertex(pos_v, norm_v, texcoord_v);
        new_mesh_ptr->vertices.push_back(vertex);
    }

    const json& indices_data = data.at("indices");
    size_t num_indices = indices_data.size();
    new_mesh_ptr->num_faces = uint32_t(num_indices / 3);
    for (int i = 0; i < num_indices; ++i)
    {
        uint32_t index = indices_data.at(i).get<uint32_t>();
        new_mesh_ptr->indices.push_back(index);
    }

    const json& submeshes_data = data.at("submeshes");
    size_t num_submeshes = submeshes_data.size();
    for (int i = 0; i < num_submeshes; ++i)
    {
        SubMesh submesh;
        json submesh_data = submeshes_data.at(i);
        
        std::string material_name = submesh_data.at("material").get<std::string>();
        uint32_t submesh_offset = submesh_data.at("offset").get<uint32_t>();
        uint32_t submesh_size = submesh_data.at("size").get<uint32_t>();

        submesh.wpMaterial = r_manager.get_material(material_name);
        submesh.index_offset = submesh_offset;
        submesh.index_count = submesh_size; 
    }

    r_manager.load_mesh(new_mesh_ptr);
    model.set_mesh_weak(new_mesh_ptr);

    return model;
}    

void JsonSceneLoader::load_materials_from_json(const json& data, ResourceManager& r_manager)
{
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        BlinnPhongMaterial mat;
        mat.name = it.key();

        // Shader should be set later
        // mat.shader = ....
        
        std::vector<float> ambient_v = it->at("ambient").get<std::vector<float>>();
        mat.ambient = Vec4f(ambient_v[0], ambient_v[1], ambient_v[2], ambient_v[3]);
        std::vector<float> diffuse_v = it->at("diffuse").get<std::vector<float>>();
        mat.diffuse = Vec4f(diffuse_v[0], diffuse_v[1], diffuse_v[2], diffuse_v[3]);     
        std::vector<float> specular_v = it->at("specular").get<std::vector<float>>();
        mat.specular = Vec4f(specular_v[0], specular_v[1], specular_v[2], specular_v[3]);
        mat.shininess = it->at("shininess").get<float>();
        mat.optical_density = it->at("optical_density").get<float>();
        mat.illumination_model =it->at("illumination_model").get<uint8_t>();

        const json& textures = it->at("textures");
        std::string scene_assets_root_path = r_manager.get_scene_assets_root();
        if (!textures.at("diffuse_map").is_null())
        {
            std::string filename = textures.at("diffuse_map").get<std::string>();
            std::shared_ptr<Texture> spTexture = TextureLoader::load_texture_from_file(scene_assets_root_path + filename);
            r_manager.load_texture(spTexture->get_name(), spTexture);
            mat.wpDiffuse_map = spTexture;
        }
            
        if (!textures.at("specular_map").is_null())
        {
            std::string filename = textures.at("specular_map").get<std::string>();
            std::shared_ptr<Texture> spTexture = TextureLoader::load_texture_from_file(scene_assets_root_path + filename);
            r_manager.load_texture(spTexture->get_name(), spTexture);
            mat.wpSpecular_map = spTexture;
        }
            
        if (!textures.at("bump_map").is_null())
        {
            std::string filename = textures.at("bump_map").get<std::string>();
            std::shared_ptr<Texture> spTexture = TextureLoader::load_texture_from_file(scene_assets_root_path + filename);
            r_manager.load_texture(spTexture->get_name(), spTexture);
            mat.wpBump_map = spTexture;
        }

        if (!textures.at("alpha_map").is_null())
        {
            std::string filename = textures.at("alpha_map").get<std::string>();
            std::shared_ptr<Texture> spTexture = TextureLoader::load_texture_from_file(scene_assets_root_path + filename);
            r_manager.load_texture(spTexture->get_name(), spTexture);
            mat.wpAlpha_map = spTexture;
        }

        r_manager.load_material(mat.name, std::make_shared<Material>(mat));
    }
}
