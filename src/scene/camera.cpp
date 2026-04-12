#include "scene/camera.h"

#include <iostream>
#include "math/math_all.h"

void Camera::print_info() const
{
    std::cout << "---- Camera ----" << std::endl;
    std::cout << "pos: " << pos_ << std::endl;
    std::cout << "center: " << center_ << std::endl;
    std::cout << "up: " << up_ << std::endl;
    std::cout << "aspect: " << aspect_ << std::endl;
    std::cout << "fovy: " << fovy_ << std::endl;
    std::cout << "near_plane: " << near_plane_ << std::endl;
    std::cout << "far_plane: " << far_plane_<< std::endl;
}