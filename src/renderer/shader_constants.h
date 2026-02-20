#ifndef __SHADER_CONSTANTS_H__
#define __SHADER_CONSTANTS_H__

#include "math/vector.hpp"

#include "renderer/render_types.hpp"

#include "scene/material.h"
#include "scene/texture.h"

struct MaterialData
{
    MaterialType type;

    // Blinn-Phong's
    Vec4f ambient;
    Vec4f diffuse;
    Vec4f specular;
    float shininess = 32.f;
    float optical_density = 1.5f;
    float transparency = 1.f;
    uint8_t illumination_model = 2;

    //PBR's

    
    //Texture Pointers
    const Texture* pDiffuse_map = nullptr;
    const Texture* pSpecular_map = nullptr;
    const Texture* pBump_map = nullptr;
    const Texture* pAlpha_map = nullptr;

};

struct ShaderConstants
{
    Uniform uniform;
    LightUniform light_uniform;
    MaterialData mat_data;
};

#endif 