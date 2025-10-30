#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "math/math_all.h"

class Material
{
public:
    Material() = default;
};

class BlinnPhongMaterial : public Material
{
public:
    BlinnPhongMaterial() = default;  

private:
    Vec3f ambient_;
    Vec3f diffuse_;
    Vec3f specular_;
    float shininess_;
};

class PBRMaterial : public Material
{
public:
    
private:

};

#endif