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
    const Uniform& context_uniform = shader_constants.uniform;
    out.clip_pos = context_uniform.MVP_matrix * input.pos;
    out.clip_w = out.clip_pos.w_;
    out.world_pos = context_uniform.model_matrix * input.pos;
    out.world_normal = context_uniform.normal_matrix * input.normal; 
    out.texcoord = input.texcoord;

    return out;
}

RGBA blinn_phong_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{

    const Uniform& uniform = shader_constants.uniform;

    const LightUniform& light_uniform = shader_constants.light_uniform;
    const std::vector<DirectionalLight>& directional_lights = light_uniform.directional_lights;
    const std::vector<PointLight>& point_lights = light_uniform.point_lights;

    const MaterialData& mat_data = shader_constants.mat_data;

    // Vec3f normal = normalize(Vec3f(input.world_normal.x_, input.world_normal.y_, input.world_normal.z_));
    
    return RGBA(mat_data.diffuse.x_, mat_data.diffuse.y_, mat_data.diffuse.z_, 1.f);
}

RGBA flat_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    return RGBA(0.f, 0.f, 1.f, 1.f);
}

RGBA wireframe_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    return RGBA(0.f, 0.f, 0.f, 1.f); // Black
}