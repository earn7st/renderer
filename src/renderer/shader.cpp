#include "math/vector.hpp"
#include "renderer/shader.h"
#include "renderer/render_types.hpp"
#include "scene/material.h"

VertexOut Shader::execute_vertex_shader(const Vertex& input, const ShaderContext& shader_context) const
{
    return vertex_shader_(input, shader_context);
}

RGBA Shader::execute_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context) const
{
    return fragment_shader_(input, shader_context);
}

VertexOut standard_vertex_shader(const Vertex& input, const ShaderContext& shader_context)
{
    VertexOut out;
    const Uniform& context_uniform = *shader_context.uniform;
    out.clip_pos = context_uniform.MVP_matrix * input.pos;
    out.clip_w = out.clip_pos.w_;
    out.world_pos = context_uniform.model_matrix * input.pos;
    // out.world_normal = 
    out.texcoord = input.texcoord;

    return out;
}

RGBA blinn_phong_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context)
{
    const BlinnPhongMaterial* mat = dynamic_cast<const BlinnPhongMaterial*>(shader_context.material);

    //Vec3f normal = normalize(Vec3f(input.world_normal.x_, input.world_normal.y_, input.world_normal.z_));

    return RGBA(mat->diffuse.x_, mat->diffuse.y_, mat->diffuse.z_, 1.f);
}

RGBA flat_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context)
{
    return RGBA(0.f, 0.f, 1.f, 1.f);
}

RGBA wireframe_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context)
{
    return RGBA(0.f, 0.f, 0.f, 1.f); // Black
}