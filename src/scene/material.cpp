#include "scene/material.h"

BlinnPhongMaterial::BlinnPhongMaterial(const Vec3f& a, const Vec3f& d, const Vec3f& s, const float& sh) :
ambient(a), diffuse(d), specular(s), shininess(sh) { }