#include "scene/texture.h"

Texture::Texture(unsigned char* data, uint32_t width, uint32_t height, const std::string& name)
: width_(width), height_(height), name_(name)
{
    pixels_.assign(data, data + width * height * 4);
}
