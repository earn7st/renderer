#include "scene/texture.h"

Texture::Texture(unsigned char* data, uint32_t width, uint32_t height, const std::string& name)
: width_(width), height_(height), name_(name)
{
    pixels_.resize(width * height);
    for (uint32_t i = 0; i < width * height; ++i)
    {
        uint32_t offset = i * 4;
        float r = data[offset] / 255.0f;
        float g = data[offset + 1] / 255.0f;
        float b = data[offset + 2] / 255.0f;
        float a = data[offset + 3] / 255.0f;
        pixels_[i] = RGBA(r, g, b, a);
    }
}

Vec3f Texture::textureRGB(float x, float y) const
{
    uint32_t row = height_ * y, col = width_ * x;
    uint32_t index = row * width_ + col;
    if (index < pixels_.size())
    {
        RGBA res = pixels_[index];
        return Vec3f(res.x_, res.y_, res.z_);
    }
    return Vec3f(0.f);    
}

void Texture::print_info() const
{
    std::cout << "Name: " << name_ << std::endl;
    std::cout << "Width: " << width_ << " Height: " <<  height_ << std::endl;
}
