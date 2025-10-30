#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include "renderer/render_types.h"
#include "renderer/shader.h"
#include "renderer/framebuffer.h"

class Rasterizer
{
public:
    void rasterize(const Varying& v0, const Varying& v1, const Varying& v2, 
                            Framebuffer* fb,
                            const Shader& shader, 
                            const Uniform& uniform, 
                            const RenderState& render_state);
    void draw_line(const Vec2f& v0, const Vec2f& v1, Framebuffer* fb, const Shader& shader);
    Vec2f viewport_transform(const Vec4f&, uint32_t screen_width, uint32_t screen_height);

private:
    void _draw_line_bresenham(const Vec2f& v0, const Vec2f& v1, Framebuffer* fb, const Shader& shader);
    //void _draw_line_DDA(const Vec2f& v0, const Vec2f& v1, const Framebuffer* fb, const Shader& shader);
};



#endif