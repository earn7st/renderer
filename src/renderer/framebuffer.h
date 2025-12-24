#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "math/math_all.h"

#ifdef _WIN32
    #include <vector>
#endif

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
    const uint32_t* get_pixels() const;

    bool set_color(uint32_t x, uint32_t y, RGBA color);
    RGBA get_color(uint32_t x, uint32_t y);

    void print();

private:
    bool initialized = false;
    uint32_t width_, height_;
    std::vector<RGBA> color_buffer_;
    std::vector<double> depth_buffer_;
};

#endif