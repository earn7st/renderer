#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "math/math_all.h"

struct Transform
{
    Vec3f scale;
    Vec3f translation;
    Vec3f rotation;
    //TODO
    // Quaternions
    Transform()
    {
        scale = Vec3f(1.0f);
        translation = Vec3f(0.0f);
        rotation = Vec3f(0.0f);
    }
    Transform(const Vec3f& _scale, const Vec3f& _translation, const Vec3f& _rotation)
    {
        scale = _scale, translation = _translation, rotation = _rotation;
    }
};

inline 
Matrix transform(const Matrix& mat, const Transform& t)
{
    Vec3f scale_v = t.scale;
    Vec3f translation_v = t.translation;
    Vec3f rotation_v = t.rotation;
    return translate(rotate(scale(mat, scale_v), rotation_v), translation_v);
}

#endif