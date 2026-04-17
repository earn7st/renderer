#ifndef __SHADER_H__
#define __SHADER_H__

#include <functional>

#include "renderer/render_types.hpp"
#include "renderer/shader_constants.h"

typedef Varying VertexOut;
typedef Varying FragmentIn;

enum ShaderType
{
    BLINN_PHONG,
    PBR
};

typedef std::function<VertexOut(const Vertex&, const ShaderConstants&)> VertexShader;
typedef std::function<RGBA(const FragmentIn&, const ShaderConstants&)> FragmentShader;

VertexOut standard_vertex_shader(const Vertex& input, const ShaderConstants& shader_constants);
RGBA blinn_phong_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants);
RGBA PBR_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants);
RGBA wireframe_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants);
RGBA flat_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants);

class Shader
{
public:
    Shader(VertexShader vs = standard_vertex_shader, FragmentShader fs = blinn_phong_fragment_shader)
    : vertex_shader_(vs), fragment_shader_(fs) {}
    Shader(const std::string& name, VertexShader vs = standard_vertex_shader, FragmentShader fs = blinn_phong_fragment_shader)
    : name_(name), vertex_shader_(vs), fragment_shader_(fs) {}

    void set_vertex_shader(VertexShader vs) { vertex_shader_ = vs; }
    void set_fragment_shader(FragmentShader fs) { fragment_shader_ = fs; }
    
    VertexOut execute_vertex_shader(const Vertex& input, const ShaderConstants& shader_constants) const;
    RGBA execute_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants) const;

    const std::string& get_name() const { return name_; }

private:
    std::string name_;
    VertexShader vertex_shader_;
    FragmentShader fragment_shader_;
};

#endif