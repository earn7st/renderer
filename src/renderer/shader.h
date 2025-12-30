#ifndef __SHADER_H__
#define __SHADER_H__

#include <functional>

#include "math/math_all.h"
#include "renderer/render_types.h"
#include "scene/material.h"

typedef Varying VertexOut;
typedef Varying FragmentIn;

enum ShaderType
{
    BLINN_PHONG,
    PBR
};

struct ShaderContext
{
    const Uniform* uniform;
    const LightUniform* light_uniform;
    const Material* material;

    void set_uniform(const Uniform* _uniform) { uniform = _uniform;}
    void set_light_uniform(const LightUniform* _light_uniform) { light_uniform = _light_uniform; }
    void set_material(const Material* _material) { material = _material; }
};

typedef std::function<VertexOut(const Vertex&, const ShaderContext&)> VertexShader;
typedef std::function<RGBA(const FragmentIn&, const ShaderContext&)> FragmentShader;

VertexOut default_vertex_shader(const Vertex& input, const ShaderContext& shader_context);
RGBA blinn_phong_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context);
RGBA wireframe_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context);

class Shader
{
public:
    Shader(VertexShader vs = default_vertex_shader, FragmentShader fs = blinn_phong_fragment_shader)
    : vertex_shader_(vs), fragment_shader_(fs) {}

    void set_vertex_shader(VertexShader vs) { vertex_shader_ = vs; }
    void set_fragment_shader(FragmentShader fs) { fragment_shader_ = fs; }
    
    VertexOut execute_vertex_shader(const Vertex& input, const ShaderContext& shader_context) const;
    RGBA execute_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context) const;

private:
    VertexShader vertex_shader_;
    FragmentShader fragment_shader_;
};

#endif