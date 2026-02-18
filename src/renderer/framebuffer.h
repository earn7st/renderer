#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include <vector>

#include "math/math_all.h"
#include "renderer/render_types.hpp"

class Framebuffer
{
public:
    Framebuffer(uint32_t w, uint32_t h);

    void resize(uint32_t w, uint32_t h);
    void clear();

    bool is_valid() const { return initialized; }
    uint32_t get_width() const { return width_; }
    uint32_t get_height() const { return height_; }
    uint32_t get_size() const { return width_ * height_; }
    const std::vector<RGBA>& get_color_data() const { return color_buffer_; }
    RGBA get_color(uint32_t x, uint32_t y) const;
    float get_depth(uint32_t x, uint32_t y) const;

    bool set_color(uint32_t x, uint32_t y, RGBA color);
    bool set_depth(uint32_t x, uint32_t y, float d);
    
    bool depth_test(uint32_t x, uint32_t y, float d) const;

    void printinfo() const;
    
private:
    bool initialized = false;
    uint32_t width_, height_;
    std::vector<RGBA> color_buffer_;
    std::vector<float> depth_buffer_;
};

#endif