#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "math/math_all.h"
#include "scene/texture.h"

#include <memory>

class Shader;

class Material
{
public:
    Material() = default;
    std::string name;
    std::weak_ptr<Shader> wpShader; 
};

class BlinnPhongMaterial : public Material
{
public:
    BlinnPhongMaterial() = default;
    BlinnPhongMaterial(BlinnPhongMaterial& mat);

    Vec4f ambient;
    Vec4f diffuse;
    Vec4f specular;
    float shininess = 32.f;
    float optical_density = 1.5f;
    float transparency = 1.f;
    uint8_t illumination_model = 2;

    std::weak_ptr<Texture> wpDiffuse_map;
    std::weak_ptr<Texture> wpSpecular_map;
    std::weak_ptr<Texture> wpBump_map;
    std::weak_ptr<Texture> wpAlpha_map;
    
};

class PBRMaterial : public Material
{
public:
    PBRMaterial() = default;

};

#endif