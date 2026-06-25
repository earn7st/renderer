#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include "renderer/render_states.h"
#include "renderer/render_types.hpp"
#include "renderer/shader.h"
#include "renderer/framebuffer.h"
#include "renderer/shader_constants.h"

#include <algorithm>

class Rasterizer
{
public:
    void rasterize(const Varying& v0, const Varying& v1, const Varying& v2,
                            Framebuffer* fb,
                            const Shader* shader,
                            const ShaderConstants& shader_constants,
                            const RenderState& render_state);

    // Tile-bounded rasterization — only draws pixels inside [tile_min, tile_max].
    void rasterize_tile(const Varying& v0, const Varying& v1, const Varying& v2,
                        Framebuffer* fb,
                        const Shader* shader,
                        const ShaderConstants& shader_constants,
                        const RenderState& render_state,
                        int tile_min_x, int tile_min_y,
                        int tile_max_x, int tile_max_y);

    void draw_line(const Vec2f& v0, const Vec2f& v1, Framebuffer* fb, const Shader* shader);
    Vec2f viewport_transform(const Vec4f&, uint32_t screen_width, uint32_t screen_height);

private:
    void _draw_line_bresenham(const Vec2f& v0, const Vec2f& v1, Framebuffer* fb, const Shader* shader);
    void _rasterize_fill(const Varying& v0, const Varying& v1, const Varying& v2,
                         Framebuffer* fb, const Shader* shader,
                         const ShaderConstants& shader_constants,
                         uint32_t screen_width, uint32_t screen_height,
                         int tile_min_x, int tile_min_y,
                         int tile_max_x, int tile_max_y);
    //void _draw_line_DDA(const Vec2f& v0, const Vec2f& v1, const Framebuffer* fb, const Shader& shader);
};



#endif