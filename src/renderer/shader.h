#ifndef __SHADER_H__
#define __SHADER_H__

#include <functional>

#include "math/math_all.h"
#include "renderer/render_types.hpp"
#include "renderer/shader_context.h"

typedef Varying VertexOut;
typedef Varying FragmentIn;

enum ShaderType
{
    BLINN_PHONG,
    PBR
};

typedef std::function<VertexOut(const Vertex&, const ShaderContext&)> VertexShader;
typedef std::function<RGBA(const FragmentIn&, const ShaderContext&)> FragmentShader;

VertexOut standard_vertex_shader(const Vertex& input, const ShaderContext& shader_context);
RGBA blinn_phong_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context);
RGBA wireframe_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context);
RGBA flat_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context);

class Shader
{
public:
    Shader(VertexShader vs = standard_vertex_shader, FragmentShader fs = blinn_phong_fragment_shader)
    : vertex_shader_(vs), fragment_shader_(fs) {}
    Shader(const std::string& name, VertexShader vs = standard_vertex_shader, FragmentShader fs = blinn_phong_fragment_shader)
    : name_(name), vertex_shader_(vs), fragment_shader_(fs) {}

    void set_vertex_shader(VertexShader vs) { vertex_shader_ = vs; }
    void set_fragment_shader(FragmentShader fs) { fragment_shader_ = fs; }
    
    VertexOut execute_vertex_shader(const Vertex& input, const ShaderContext& shader_context) const;
    RGBA execute_fragment_shader(const FragmentIn& input, const ShaderContext& shader_context) const;

    const std::string& get_name() const { return name_; }

private:
    std::string name_;
    VertexShader vertex_shader_;
    FragmentShader fragment_shader_;
};

#endif