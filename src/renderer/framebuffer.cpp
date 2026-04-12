#include "math/color.h"
#include "renderer/framebuffer.h"

Framebuffer::Framebuffer(uint32_t w, uint32_t h)
{
    initialized = true;
    width_ = w, height_ = h;
    color_buffer_.resize(w * h, Color::GRAY);
    depth_buffer_.resize(w * h, std::numeric_limits<float>::max());
}

void Framebuffer::resize(uint32_t w, uint32_t h)
{
    width_ = w, height_ = h;
    color_buffer_.resize(w * h, Color::GRAY);
    depth_buffer_.resize(w * h, std::numeric_limits<float>::max());
}

void Framebuffer::clear()
{
    std::fill(color_buffer_.begin(), color_buffer_.end(), Color::GRAY);
    std::fill(depth_buffer_.begin(), depth_buffer_.end(), std::numeric_limits<float>::max());
}

RGBA Framebuffer::get_color(uint32_t x, uint32_t y) const
{
    return color_buffer_[y * width_ + x];
}

float Framebuffer::get_depth(uint32_t x, uint32_t y) const
{
    return depth_buffer_[y * width_ + x];
} 

bool Framebuffer::set_color(uint32_t x, uint32_t y, RGBA color)
{
    if(x < width_ && y < height_)
    {
        color_buffer_[y * width_ + x] = color;
        return true;
    }
    return false;
}

bool Framebuffer::set_depth(uint32_t x, uint32_t y, float d)
{
    depth_buffer_[y * width_ + x] = d;
    return true;
}

bool Framebuffer::depth_test(uint32_t x, uint32_t y, float d) const
{
    return get_depth(x, y) > d;
}

void Framebuffer::printinfo() const
{
    std::cout << "Framebuffer Info:" << std::endl;
    std::cout << width_ << " " << height_ << std::endl;
}
