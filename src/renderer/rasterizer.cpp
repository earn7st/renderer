#include "renderer/rasterizer.h"
#include "math/math_all.h"

void Rasterizer::rasterize(const Varying& v0, const Varying& v1, const Varying& v2,
                            Framebuffer* fb,
                            const Shader* shader,
                            const ShaderConstants& shader_constants,
                            const RenderState& render_state)
{
    // Perspective Division
    Vec4f v0_ndc_pos = v0.clip_pos / v0.clip_pos.w_;
    Vec4f v1_ndc_pos = v1.clip_pos / v1.clip_pos.w_;
    Vec4f v2_ndc_pos = v2.clip_pos / v2.clip_pos.w_;

    uint32_t screen_width = fb->get_width();
    uint32_t screen_height = fb->get_height();

    // Viewport Transform
    Vec2f v0_screen_pos = viewport_transform(v0_ndc_pos, screen_width, screen_height);
    Vec2f v1_screen_pos = viewport_transform(v1_ndc_pos, screen_width, screen_height);
    Vec2f v2_screen_pos = viewport_transform(v2_ndc_pos, screen_width, screen_height);

    if (render_state.polygon_mode == LINE)
    {
        draw_line(v0_screen_pos, v1_screen_pos, fb, shader);
        draw_line(v1_screen_pos, v2_screen_pos, fb, shader);
        draw_line(v2_screen_pos, v0_screen_pos, fb, shader);
    }
    else if (render_state.polygon_mode == FILL)
    {
        _rasterize_fill(v0, v1, v2, fb, shader, shader_constants,
                        screen_width, screen_height,
                        0, 0,
                        (int)screen_width, (int)screen_height);  // exclusive upper bound
    }
}

void Rasterizer::rasterize_tile(const Varying& v0, const Varying& v1, const Varying& v2,
                                 Framebuffer* fb,
                                 const Shader* shader,
                                 const ShaderConstants& shader_constants,
                                 const RenderState& render_state,
                                 int tile_min_x, int tile_min_y,
                                 int tile_max_x, int tile_max_y)
{
    if (render_state.polygon_mode != FILL) return;  // tile path only for FILL

    Vec4f v0_ndc_pos = v0.clip_pos / v0.clip_pos.w_;
    Vec4f v1_ndc_pos = v1.clip_pos / v1.clip_pos.w_;
    Vec4f v2_ndc_pos = v2.clip_pos / v2.clip_pos.w_;

    uint32_t screen_width = fb->get_width();
    uint32_t screen_height = fb->get_height();

    Vec2f v0_sp = viewport_transform(v0_ndc_pos, screen_width, screen_height);
    Vec2f v1_sp = viewport_transform(v1_ndc_pos, screen_width, screen_height);
    Vec2f v2_sp = viewport_transform(v2_ndc_pos, screen_width, screen_height);

    _rasterize_fill(v0, v1, v2, fb, shader, shader_constants,
                    screen_width, screen_height,
                    tile_min_x, tile_min_y,
                    tile_max_x, tile_max_y);
}

void Rasterizer::_rasterize_fill(const Varying& v0, const Varying& v1, const Varying& v2,
                                  Framebuffer* fb, const Shader* shader,
                                  const ShaderConstants& shader_constants,
                                  uint32_t screen_width, uint32_t screen_height,
                                  int tile_min_x, int tile_min_y,
                                  int tile_max_x, int tile_max_y)
{
    // Compute NDC and screen positions once (same as original rasterize())
    Vec4f v0_ndc = v0.clip_pos / v0.clip_pos.w_;
    Vec4f v1_ndc = v1.clip_pos / v1.clip_pos.w_;
    Vec4f v2_ndc = v2.clip_pos / v2.clip_pos.w_;

    Vec2f v0_sp = viewport_transform(v0_ndc, screen_width, screen_height);
    Vec2f v1_sp = viewport_transform(v1_ndc, screen_width, screen_height);
    Vec2f v2_sp = viewport_transform(v2_ndc, screen_width, screen_height);

    // Precompute edge function denominators for barycentrics
    // (exactly matching original rasterize())
    float edge_v1_02 = edge_function(v1_sp, v0_sp, v2_sp);  // E_02(v1)
    float edge_v2_01 = edge_function(v2_sp, v0_sp, v1_sp);  // E_01(v2)

    // Bounding box (exactly matching original)
    uint32_t min_x = (uint32_t)std::max(0.0f, std::floor(std::min({v0_sp.x_, v1_sp.x_, v2_sp.x_})));
    uint32_t min_y = (uint32_t)std::max(0.0f, std::floor(std::min({v0_sp.y_, v1_sp.y_, v2_sp.y_})));
    uint32_t max_x = (uint32_t)std::min((float)(screen_width - 1),  std::ceil(std::max({v0_sp.x_, v1_sp.x_, v2_sp.x_})));
    uint32_t max_y = (uint32_t)std::min((float)(screen_height - 1), std::ceil(std::max({v0_sp.y_, v1_sp.y_, v2_sp.y_})));

    // Clamp to tile bounds
    min_x = std::max(min_x, (uint32_t)tile_min_x);
    min_y = std::max(min_y, (uint32_t)tile_min_y);
    max_x = std::min(max_x, (uint32_t)tile_max_x);
    max_y = std::min(max_y, (uint32_t)tile_max_y);

    for (uint32_t y = min_y; y < max_y; ++y)
    {
        for (uint32_t x = min_x; x < max_x; ++x)
        {
            float cx = (float)x + 0.5f;
            float cy = (float)y + 0.5f;
            Vec2f p(cx, cy);

            if (inside_triangle(p, v0_sp, v1_sp, v2_sp))
            {
                Vec3f bary = compute_barycentric_coord_2D(p, v0_sp, v1_sp, v2_sp, edge_v1_02, edge_v2_01);
                float alpha = bary.x_, beta = bary.y_, gamma = bary.z_;

                float Z = alpha * v1.clip_w * v2.clip_w + beta * v0.clip_w * v2.clip_w + gamma * v0.clip_w * v1.clip_w;
                float alpha_c = (alpha * v1.clip_w * v2.clip_w) / Z;
                float beta_c  = (beta  * v0.clip_w * v2.clip_w) / Z;
                float gamma_c = (gamma * v0.clip_w * v1.clip_w) / Z;

                Varying point = interpolate(v0, v1, v2, alpha_c, beta_c, gamma_c);

                float d = interpolate(v0_ndc, v1_ndc, v2_ndc, alpha, beta, gamma).z_;
                if (fb->depth_test(x, y, d))
                {
                    fb->set_depth(x, y, d);
                    RGBA color = shader->execute_fragment_shader(point, shader_constants);
                    fb->set_color(x, y, color);
                }
            }
        }
    }
}

// flip vertically
// Left Up corner as (0, 0)
Vec2f Rasterizer::viewport_transform(const Vec4f& v, uint32_t screen_width, uint32_t screen_height)
{
    float ndc_x = v.x_;
    float ndc_y = v.y_;

    float screen_x = (v.x_ + 1) * screen_width * 0.5f;
    float screen_y = (1 - v.y_) * screen_height * 0.5f;

    return Vec2f(screen_x, screen_y);
}

void Rasterizer::draw_line(const Vec2f& v0, const Vec2f& v1, Framebuffer* fb, const Shader* shader)
{
    _draw_line_bresenham(v0, v1, fb, shader);
    //_draw_line_DDA(v0, v1, shader, render_state);
    return ;
}

void Rasterizer::_draw_line_bresenham(const Vec2f& v0, const Vec2f& v1, Framebuffer* fb, const Shader* shader)
{
    uint32_t screen_width = fb->get_width();
    uint32_t screen_height = fb->get_height();

    FragmentIn dummy_fragin = FragmentIn();
    ShaderConstants dummy_shader_constants = ShaderConstants();

    int x0 = int(v0.x_), y0 = int(v0.y_);
    int x1 = int(v1.x_), y1 = int(v1.y_);

    int cx = x0 < x1 ? 1 : -1;
    int cy = y0 < y1 ? 1 : -1;
    int cnt = 0;
    int dy = abs(y1 - y0);
    int dx = abs(x1 - x0);

    int x = x0, y = y0;

    if(dy < dx){
        for(; cx > 0 ? x <= std::min(screen_width - 1, (uint32_t)x1) : x >= std::max(0, x1); x += cx)
        {
            cnt += 2 * dy;
            
            if(cnt >= dx)
            {
                y += cy;
                cnt -= 2 * dx;
            }
            RGBA color = shader->execute_fragment_shader(dummy_fragin, dummy_shader_constants);
            fb->set_color(x, y, color);
        }
    } else {
        for(; cy > 0 ? y <= std::min(screen_height - 1, (uint32_t)y1) : y >= std::max(0, y1); y += cy)
        {
            cnt += 2 * dx;
            
            if(cnt >= dy)
            {
                x += cx;
                cnt -= 2 * dy;
            }
            RGBA color = shader->execute_fragment_shader(dummy_fragin, dummy_shader_constants);
            fb->set_color(x, y, color);
        }
    }

    return ;
}
