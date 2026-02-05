#include "scene/texture.h"

Texture::Texture(unsigned char* data, uint32_t width, uint32_t height, const std::string& name)
: width_(width), height_(height), name_(name)
{
    pixels_.assign(data, data + width * height * 4);
}

void Texture::print_info() const
{
    std::cout << "Name: " << name_ << std::endl;
    std::cout << "Width: " << width_ << " Height: " <<  height_ << std::endl;
}
