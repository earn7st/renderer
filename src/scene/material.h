#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "math/math_all.h"
#include "renderer/shader.h"

class Material
{
public:
    Material() = default;
    std::string name;
    Shader* shader; 
};

class BlinnPhongMaterial : public Material
{
public:
    BlinnPhongMaterial() = default;  
    BlinnPhongMaterial(const Vec3f&, const Vec3f&, const Vec3f&, const float&);

    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    float shininess;
};

class PBRMaterial : public Material
{
public:
    PBRMaterial() = default;

};

#endif