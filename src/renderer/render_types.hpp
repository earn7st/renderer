#ifndef __RENDER_TYPES_H__
#define __RENDER_TYPES_H__

#include <vector>

#include "math/math_all.h"
#include "scene/light.h"

struct Uniform
{
    // per-frame
    Matrix view_matrix = Matrix::Identity;  
    Matrix projection_matrix = Matrix::Identity;    
    Matrix VP_matrix = Matrix::Identity;
    Vec3f world_camera_position = Vec3f(0.0f);  

    // per-model
    Matrix model_matrix = Matrix::Identity;

    // per-submesh
    Matrix sub_mesh_matrix = Matrix::Identity;  // local transform
    Matrix MVP_matrix = Matrix::Identity;
};

struct LightUniform
{
    std::vector<DirectionalLight> directional_lights;
    std::vector<PointLight> point_lights; 
};

struct Varying
{
    Vec4f clip_pos;
    Vec4f world_pos;
    Vec4f world_normal;
    Vec2f texcoord;
    float clip_w;
};

inline
Varying interpolate(const Varying& v0, const Varying& v1, const Varying& v2, float alpha, float beta, float gamma)
{
    Varying result;
    
    result.world_pos = interpolate(v0.world_pos, v1.world_pos, v2.world_pos, alpha, beta, gamma);
    result.world_normal = interpolate(v0.world_normal, v1.world_normal, v2.world_normal, alpha, beta, gamma);
    result.texcoord = interpolate(v0.texcoord, v1.texcoord, v2.texcoord, alpha, beta, gamma);

    return result;
}

struct Vertex
{
    Vertex() = default;
    Vertex(const Vec4f& _pos, const Vec4f& _normal, const Vec2f& _texcoord)
    : pos(_pos), normal(_normal), texcoord(_texcoord) {}
    Vertex(const Vertex& v) : pos(v.pos), normal(v.normal), texcoord(v.texcoord){}
    
    Vec4f pos;
    Vec4f normal;
    Vec2f texcoord;
};

#endif