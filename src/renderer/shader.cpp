#include "renderer/shader.h"
#include "renderer/render_types.h"

VertexOut Shader::execute_vertex_shader(const Vertex& input, const Uniform& uniform) const
{
    return vertex_shader_(input, uniform);
}

RGBA Shader::execute_fragment_shader(const FragmentIn& input, const Uniform& uniform) const
{
    return fragment_shader_(input, uniform);
}

VertexOut default_vertex_shader(const Vertex& input, const Uniform& uniform)
{
    VertexOut out;
    out.clip_pos = uniform.MVP_matrix * input.pos;
    out.clip_w = out.clip_pos.w_;
    out.world_pos = uniform.model_matrix * input.pos;
    // out.world_normal = 
    out.texcoord = input.texcoord;

    return out;
}

RGBA blinn_phong_fragment_shader(const FragmentIn& input, const Uniform& uniform)
{
    return RGBA(0, 0, 255, 255);
}

RGBA wireframe_fragment_shader(const FragmentIn& input, const Uniform& uniform)
{
    return RGBA(0, 0, 0, 255); // Black
}