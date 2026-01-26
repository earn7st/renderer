#include <fstream>
#include "json.hpp"
#include "math/math_all.h"
#include "renderer/render_types.h"
#include "scene/scene_loader.h"


using json = nlohmann::json;

int JsonSceneLoader::load_scene(const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager)
{
    std::string scene_filepath = scene_context_path + "scene.json";

    std::ifstream f(scene_filepath);
    json data = json::parse(f);

    if (data.contains("camera")) 
    {
        json& camera = data.at("camera");
        if (load_camera(camera, scene) != 0)
        {
            std::cerr << "JsonSceneLoader::load_scene : Failed Loading Camera Data\n";
        }   
    }

    if (data.contains("object")) 
    {
        json& object_data = data.at("object");
        if (load_object(object_data, scene_context_path, scene, r_manager) != 0)
        {
            std::cerr << "JsonSceneLoader::load_objects : Failed Loading Objects Data\n";
            return -1;
        }
    } else if (data.contains("objects"))
    {
        // TODO: Multiple Objects
        // Avoid Repetitve Model Loading
    }

    if (data.contains("lights")) 
    {
        json& lights = data.at("lights");
        if (load_lights(lights, scene) != 0)
        {
            std::cerr << "JsonSceneLoader::load_lights : Failed Loading Lights Data\n";
            return -1;
        }
    }    

    return 0;
}

int JsonSceneLoader::load_camera(const json& data, Scene& scene)
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

int JsonSceneLoader::load_object(const json& object_data, const std::string& scene_context_path, Scene& scene, ResourceManager& r_manager)
{

    std::string filename = object_data.at("filename").get<std::string>();

    std::string model_filepath = scene_context_path + filename;
    std::ifstream f(model_filepath);
    json model_data = json::parse(f);

    Model model = load_model(model_data, scene, r_manager);

    if (object_data.contains("transform"))
    {
        json transform_data = object_data.at("transform");

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

Model JsonSceneLoader::load_model(const json& data, Scene& scene, ResourceManager& r_manager)
{
    Model model;
    if (data.at("group").get<bool>() == false)
    {
        std::shared_ptr<Mesh> new_mesh_ptr = std::make_shared<Mesh>();

        json positions_data = data.at("positions");
        json normals_data = data.at("normals");
        json texcoords_data = data.at("texcoords");
        size_t num_vertices = positions_data.size();
        for (int i = 0; i < num_vertices; ++i)
        {
            std::vector<float> pos = positions_data.at(i).get<std::vector<float>>();
            Vec4f pos_v(pos[0], pos[1], pos[2], 1.0f);
            
            std::vector<float> normal = normals_data.at(i).get<std::vector<float>>();
            Vec4f normal_v(normal[0], normal[1], normal[2], 0.0f);

            if(!texcoords_data.is_null())
            {
                std::vector<float> texcoord = texcoords_data.at(i).get<std::vector<float>>();
                Vec2f texcoord_v(texcoord[0], texcoord[1]);

                Vertex vertex(pos_v, normal_v, texcoord_v);
                new_mesh_ptr->vertices.push_back(vertex);
            } else 
            {
                Vertex vertex(pos_v, normal_v, Vec2f(0.0f));
                new_mesh_ptr->vertices.push_back(vertex);
            }
        }
        new_mesh_ptr->num_vertices = num_vertices;

        json faces_data = data.at("faces");
        size_t num_faces = faces_data.size();
        for (int i = 0; i < num_faces; ++i)
        {
            std::vector<int> face = faces_data.at(i).get<std::vector<int>>();
            for (int j = 0; j < 3; ++j)
            {
                new_mesh_ptr->indices.push_back(face[j]);
            }
        }
        new_mesh_ptr->num_faces = num_faces;

        r_manager.load_mesh(new_mesh_ptr);
        model.set_mesh_weak(new_mesh_ptr);
        
        // TODO : Multiple submeshes with different materials
        SubMesh sub_mesh0;
        sub_mesh0.offset = 0;
        sub_mesh0.size = num_faces * 3;
        if (data.contains("material"))
        {
            json material_data = data.at("material");
            
            std::string material_type = material_data.at("type").get<std::string>();
            if (material_type == "BlinnPhong")
            {
                std::vector<float> ambient = material_data.at("ambient").get<std::vector<float>>();
                Vec3f ambient_v(ambient[0], ambient[1], ambient[2]);
                
                std::vector<float> diffuse = material_data.at("diffuse").get<std::vector<float>>();
                Vec3f diffuse_v(diffuse[0], diffuse[1], diffuse[2]);
                
                std::vector<float> specular = material_data.at("specular").get<std::vector<float>>();
                Vec3f specular_v(specular[0], specular[1], specular[2]);

                float shininess = material_data.at("shininess").get<float>();

                BlinnPhongMaterial material(ambient_v, diffuse_v, specular_v, shininess);
                std::shared_ptr<BlinnPhongMaterial> new_material_ptr = std::make_shared<BlinnPhongMaterial>();
                r_manager.load_material(new_material_ptr);
                sub_mesh0.wpMaterial = new_material_ptr;
            }
            else if(material_type == "PBR")
            {
                // ...
            }
        }

        model.add_sub_mesh(sub_mesh0);
        
        return model;
    }
    return model;
}    

int JsonSceneLoader::load_lights(const json& data, Scene& scene)
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
