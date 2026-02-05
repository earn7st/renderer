#ifndef __SHADER_CONTEXT_H__
#define __SHADER_CONTEXT_H__

#include "renderer/render_types.h"

class Material;

struct ShaderContext
{
    const Uniform* uniform;
    const LightUniform* light_uniform;
    const Material* material;

    void set_uniform(const Uniform* _uniform) { uniform = _uniform;}
    void set_light_uniform(const LightUniform* _light_uniform) { light_uniform = _light_uniform; }
    void set_material(const Material* _material) { material = _material; }
};


#endif