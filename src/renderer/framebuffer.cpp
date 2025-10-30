#include "renderer/framebuffer.h"

Framebuffer::Framebuffer(uint32_t w, uint32_t h)
{
    initialized = true;
    width_ = w, height_ = h;
    color_buffer_.resize(w * h, RGBA(255, 0, 255, 255));
    depth_buffer_.resize(w * h, 0.0f);
}

void Framebuffer::resize(uint32_t w, uint32_t h)
{
    width_ = w, height_ = h;
    color_buffer_.resize(w * h, RGBA(255, 0, 255, 255));
    depth_buffer_.resize(w * h, 0.0f);
}

void Framebuffer::clear()
{
    std::fill(color_buffer_.begin(), color_buffer_.end(), RGBA(255, 0, 255, 255));
    std::fill(depth_buffer_.begin(), depth_buffer_.end(), std::numeric_limits<double>::lowest());
}
const uint32_t* Framebuffer::get_pixels() const 
{
    return reinterpret_cast<const uint32_t*> (color_buffer_.data());
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

RGBA Framebuffer::get_color(uint32_t x, uint32_t y)
{
    return color_buffer_[y * width_ + x];
}

void Framebuffer::print()
{
    std::cout << "Framebuffer Info:" << std::endl;
    std::cout << width_ << " " << height_ << std::endl;
}