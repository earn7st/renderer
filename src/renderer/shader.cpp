#include "renderer/shader.h"
#include "renderer/render_types.h"

VertexOut Shader::execute_vertex_shader(const Vertex& input, const ShaderContext& shader_context) const
{
    return vertex_shader_(input, shader_context);
}

RGBA Shader::execute_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context) const
{
    return fragment_shader_(input, shader_context);
}

VertexOut default_vertex_shader(const Vertex& input, const ShaderContext& shader_context)
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
    return RGBA(0, 0, 255, 255);
}

RGBA wireframe_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context)
{
    return RGBA(0, 0, 0, 255); // Black
}