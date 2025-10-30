#ifndef __RENDER_TYPES_H__
#define __RENDER_TYPES_H__

#include "math/math_all.h"

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

    // material

    // lighting
};

struct Varying
{
    Vec4f clip_pos;
    Vec4f world_pos;
    Vec4f world_normal;
    Vec2f texcoord;
    float clip_w;
};

enum PolygonMode {
    POINT,
    LINE,
    FILL
};

enum ShaderType
{
    BLINN_PHONG,
    PBR
};

struct RenderState
{
    PolygonMode polygon_mode = FILL;

    ShaderType shader_type = BLINN_PHONG;
};

struct ClearStates
{
    bool color_flag = false;
    bool depth_flag = false;
    Vec3f clear_color = Vec3f(0.0f);
    double clear_depth = 0.0f;
};

#endif