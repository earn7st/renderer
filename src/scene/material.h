#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "math/math_all.h"
#include "renderer/shader.h"
#include "scene/texture.h"

#include <memory>

class Material
{
public:
    Material() = default;
    Material(const Material&);
    virtual void print_info() const = 0;

    std::string name;
    std::weak_ptr<Shader> wpShader; 
};

class BlinnPhongMaterial : public Material
{
public:
    BlinnPhongMaterial() = default;
    BlinnPhongMaterial(const BlinnPhongMaterial& mat);

    void print_info() const override;

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