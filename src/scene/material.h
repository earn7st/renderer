#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "math/math_all.h"
#include "renderer/shader.h"
#include "scene/texture.h"

#include <memory>

enum MaterialType
{
    BLINN_PHONG_MAT,
    PBR_MAT
};

class Material
{
public:
    Material() = default;
    Material(MaterialType _type) : type(_type) {}
    Material(const Material&);
    virtual void print_info() const = 0;
    MaterialType get_type() const { return type; }

    std::string name;
    MaterialType type;
    std::weak_ptr<Shader> wpShader; 
};

class BlinnPhongMaterial : public Material
{
public:
    BlinnPhongMaterial() : Material(BLINN_PHONG_MAT) {}
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
    PBRMaterial() : Material(PBR_MAT) {}
    PBRMaterial(const PBRMaterial& mat);

    void print_info() const override;

    Vec4f albedo;
    float roughness;
    float metallic;

    std::weak_ptr<Texture> wpAlbedo_map;
    std::weak_ptr<Texture> wpRoughness_map;
    std::weak_ptr<Texture> wpMetallic_map;
    std::weak_ptr<Texture> wpNormal_map;
    std::weak_ptr<Texture> wpAO_map;
};

#endif