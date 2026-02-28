#include "math/vector.hpp"
#include "renderer/shader.h"
#include "renderer/render_types.hpp"

VertexOut Shader::execute_vertex_shader(const Vertex& input, const ShaderConstants& shader_constants) const
{
    return vertex_shader_(input, shader_constants);
}

RGBA Shader::execute_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants) const
{
    return fragment_shader_(input, shader_constants);
}

VertexOut standard_vertex_shader(const Vertex& input, const ShaderConstants& shader_constants)
{
    VertexOut out;
    const Uniform& uniform = shader_constants.uniform;
    out.clip_pos = uniform.MVP_matrix * input.pos;
    out.clip_w = out.clip_pos.w_;
    out.world_pos = uniform.model_matrix * input.pos;
    out.world_normal = uniform.normal_matrix * input.normal; 
    out.texcoord = input.texcoord;

    return out;
}

RGBA blinn_phong_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    // Get References of Uniform
    const Uniform& uniform = shader_constants.uniform;
    const LightUniform& light_uniform = shader_constants.light_uniform;
    const MaterialData& mat_data = shader_constants.mat_data;

    // Preparation
    Vec3f frag_pos = Vec3f(input.world_pos.x_, input.world_pos.y_, input.world_pos.z_);
    Vec3f camera_pos = uniform.world_camera_position; 
    Vec3f view_dir = normalize(camera_pos - frag_pos); 
    Vec3f normal = normalize(Vec3f(input.world_normal.x_, input.world_normal.y_, input.world_normal.z_));

    Vec3f ambient = Vec3f(mat_data.ambient.x_, mat_data.ambient.y_, mat_data.ambient.z_);
    Vec3f diffuse_total;
    Vec3f specular_total;

    // Directional Lights
    for (const auto& light:light_uniform.directional_lights)
    {
        Vec3f light_dir = normalize(-light.direction);
        
        // Diffuse
        float diff = std::max(dot(normal, light_dir), 0.f);
        diffuse_total += Vec3f(mat_data.diffuse.x_, mat_data.diffuse.y_, mat_data.diffuse.z_) * light.color * diff;

        // Specular
        Vec3f halfway_dir = normalize(view_dir + light_dir);
        float spec = std::pow(std::max(dot(normal, halfway_dir), 0.0f), mat_data.shininess);
        specular_total += Vec3f(mat_data.specular.x_, mat_data.specular.y_, mat_data.specular.z_) * light.color * spec;
    }
    
    Vec3f final_color = ambient + diffuse_total + specular_total;

    final_color.x_ = std::min(1.f, final_color.x_);
    final_color.y_ = std::min(1.f, final_color.y_);
    final_color.z_ = std::min(1.f, final_color.z_);

    return RGBA(final_color, 1.f);
}

RGBA flat_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    return RGBA(0.f, 0.f, 1.f, 1.f);
}

RGBA wireframe_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    return RGBA(0.f, 0.f, 0.f, 1.f); // Black
}