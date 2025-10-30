#include "renderer/rasterizer.h"
#include "math/math_all.h"

void Rasterizer::rasterize(const Varying& v0, const Varying& v1, const Varying& v2, 
                            Framebuffer* fb,
                            const Shader& shader, 
                            const Uniform& uniform, 
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

    } else if (render_state.polygon_mode == FILL)
    {
        float edge_func_result_v1 = edge_function(v1_screen_pos, v0_screen_pos, v2_screen_pos);
        float edge_func_result_v2 = edge_function(v2_screen_pos, v0_screen_pos, v1_screen_pos);

        uint32_t min_x = std::max((uint32_t)0, (uint32_t)std::floor(std::min({v0_screen_pos.x_, v1_screen_pos.x_, v2_screen_pos.x_})));
        uint32_t min_y = std::max((uint32_t)0, (uint32_t)std::floor(std::min({v0_screen_pos.y_, v1_screen_pos.y_, v2_screen_pos.y_})));
        uint32_t max_x = std::min(screen_width - 1, (uint32_t)std::ceil(std::max({v0_screen_pos.x_, v1_screen_pos.x_, v2_screen_pos.x_})));
        uint32_t max_y = std::min(screen_height - 1, (uint32_t)std::ceil(std::max({v0_screen_pos.y_, v1_screen_pos.y_, v2_screen_pos.y_})));

        for(uint32_t x = min_x; x < max_x; ++x)
        {
            for(uint32_t y = min_y; y < max_y; ++y)
            {
                float cx = x + 0.5;
                float cy = y + 0.5;
                if(inside_triangle(Vec2f(cx, cy), v0_screen_pos, v1_screen_pos, v2_screen_pos))
                {
                    Vec3f bary = compute_barycentric_coord_2D(Vec2f(cx, cy), v0_screen_pos, v1_screen_pos, v2_screen_pos, edge_func_result_v1, edge_func_result_v2);
                    float alpha = bary.x_, beta = bary.y_, gamma = bary.z_;
                    // TODO : correct interpolation
                    

                }
            }
        }
    }

    return ;
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

void Rasterizer::draw_line(const Vec2f& v0, const Vec2f& v1, Framebuffer* fb, const Shader& shader)
{
    _draw_line_bresenham(v0, v1, fb, shader);
    //_draw_line_DDA(v0, v1, shader, render_state);
    return ;
}

void Rasterizer::_draw_line_bresenham(const Vec2f& v0, const Vec2f& v1, Framebuffer* fb, const Shader& shader)
{
    uint32_t screen_width = fb->get_width();
    uint32_t screen_height = fb->get_height();

    FragmentIn dummy_fragin = FragmentIn();
    Uniform dummy_uniform = Uniform();

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
            RGBA color = shader.execute_fragment_shader(dummy_fragin, dummy_uniform);
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
            RGBA color = shader.execute_fragment_shader(dummy_fragin, dummy_uniform);
            fb->set_color(x, y, color);
        }
    }

    return ;
}
