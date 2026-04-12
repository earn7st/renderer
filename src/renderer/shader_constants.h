#ifndef __SHADER_CONSTANTS_H__
#define __SHADER_CONSTANTS_H__

#include "math/vector.hpp"

#include "renderer/render_types.hpp"

#include "scene/texture.h"

struct MaterialData
{
    // BlinnPhong's
    Vec4f ambient;
    Vec4f diffuse;
    Vec4f specular;
    float shininess = 32.f;
    float optical_density = 1.5f;
    float transparency = 1.f;
    uint8_t illumination_model = 2;

    //PBR's
    Vec4f albedo;
    float roughness;
    float metallic;
    
    //Texture Pointers
    // BlinnPhong's
    const Texture* pDiffuse_map = nullptr;
    const Texture* pSpecular_map = nullptr;
    const Texture* pBump_map = nullptr;
    const Texture* pAlpha_map = nullptr;   
    // PBR's
    const Texture* pAlbedo_map = nullptr;
    const Texture* pRoughness_map = nullptr;
    const Texture* pMetallic_map = nullptr;
    const Texture* pNormal_map = nullptr;
    const Texture* pAO_map = nullptr;
    
};

struct ShaderConstants
{
    Uniform uniform;
    LightUniform light_uniform;
    MaterialData mat_data;
};

#endif 