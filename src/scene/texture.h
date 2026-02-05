#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <vector>
#include <string>

#include "math/math_all.h"

class Texture
{
public:
    Texture() = default;
    Texture(unsigned char* data, uint32_t width, uint32_t height, const std::string& name);

    const std::string& get_name() const { return name_; }

    void print_info() const;

private:
    std::string name_;
    uint32_t width_, height_;
    std::vector<RGBA> pixels_;

};

#endif