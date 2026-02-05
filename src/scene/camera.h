#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "math/math_all.h"

#define DEFAULT_CAMERA_POS Vec3f(0, 0, -5)
#define DEFAULT_CAMERA_CENTER Vec3f(0, 0, 0)
#define WORLD_UP Vec3f(0, 1, 0)
#define DEFAULT_CAMERA_ASPECT 1.333f
#define DEFAULT_CAMERA_FOVY 60.0f
#define DEFAULT_CAMERA_NEAR_PLANE 0.1f
#define DEFAULT_CAMERA_FAR_PLANE 10.0f


class Camera
{
public:
    Camera(const Vec3f& pos = DEFAULT_CAMERA_POS, 
        const Vec3f& center = DEFAULT_CAMERA_CENTER, 
        const Vec3f& up = WORLD_UP, 
        float aspect = DEFAULT_CAMERA_ASPECT,
        float fovy = DEFAULT_CAMERA_FOVY,
        float near_plane = DEFAULT_CAMERA_NEAR_PLANE,
        float far_plane = DEFAULT_CAMERA_FAR_PLANE)
    : pos_(pos), center_(center), up_(up), aspect_(aspect), 
    fovy_(fovy), near_plane_(near_plane), far_plane_(far_plane){ }

    // Getters
    Vec3f get_pos() const { return pos_; }
    Vec3f get_center() const { return center_; }
    Vec3f get_up() const { return up_; }
    float get_aspect() const { return aspect_; }
    float get_fovy() const { return fovy_; }
    float get_near_plane() const { return near_plane_; }
    float get_far_plane() const { return far_plane_; }

    void print_info() const;

private:
    Vec3f pos_ ;
    Vec3f center_;
    Vec3f up_ ;         //FPS Camera
    float aspect_;      // width/height

    float fovy_;
    float near_plane_;
    float far_plane_;
};

#endif