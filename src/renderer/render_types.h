#ifndef __RENDER_TYPES_H__
#define __RENDER_TYPES_H__

#include <vector>

#include "math/math_all.h"
#include "scene/light.h"

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

enum PolygonMode
{
    LINE,
    FILL
};

enum CullMode
{
    BACK
};

struct RenderState
{
    PolygonMode polygon_mode = FILL;
    CullMode cull_mode = BACK;
    bool depth_test = true;
};

struct ClearStates
{
    bool color_flag = false;
    bool depth_flag = false;
    Vec3f clear_color = Vec3f(0.0f);
    double clear_depth = 0.0f;
};

#endif