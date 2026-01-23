#ifndef __RENDER_STATES_H__
#define __RENDER_STATES_H__

#include "math/vector.hpp"

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