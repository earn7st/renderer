#ifndef __GEOMETRY_HPP__
#define __GEOMETRY_HPP__

#include <algorithm>

#include "vector.hpp"

inline
bool inside_triangle(const Vec2f& p, const Vec2f& v_a, const Vec2f& v_b, const Vec2f& v_c)
{
    float max_y = std::max({v_a.y_, v_b.y_, v_c.y_});
    Vec2f e_ab = v_b - v_a;
    Vec2f e_bc = v_c - v_b;
    Vec2f e_ca = v_a - v_c;
    Vec2f ap = p - v_a;
    Vec2f bp = p - v_b;
    Vec2f cp = p - v_c;

    float cross_product_a = cross(e_ab, ap);
    if(cross_product_a == 0) 
        if(e_ab.y_ < 0 || (e_ab.y_ == 0 && max_y == v_a.y_))
            return true;
    float cross_product_b = cross(e_bc, bp);
    if(cross_product_b == 0) 
        if(e_bc.y_ < 0 || (e_bc.y_ == 0 && max_y == v_b.y_))
            return true;
    float cross_product_c = cross(e_ca, cp);
    if(cross_product_c == 0) 
        if(e_ca.y_ < 0 || (e_ca.y_ == 0 && max_y == v_c.y_))
            return true;

    bool x_a = cross_product_a < 0 ? true : false;
    bool x_b = cross_product_b < 0 ? true : false;
    bool x_c = cross_product_c < 0 ? true : false;

    return ((x_a ^ x_b) == 0 && (x_b ^ x_c) == 0);
}

// edge: va -> vb
inline
float edge_function(const Vec2f& v, const Vec2f& v_a, const Vec2f& v_b)
{
    return (v_a.y_ - v_b.y_) * v.x_ + (v_b.x_ - v_a.x_) * v.y_ + v_a.x_ * v_b.y_ - v_b.x_ * v_a.y_;
}

// compute f_ab_v_c and f_ca_v_b multiple times
inline
Vec3f compute_barycentric_coord_2D(const Vec2f& v, const Vec2f& v_a, const Vec2f& v_b, const Vec2f& v_c)
{
    float f_ab_v_c = edge_function(v_c, v_a, v_b);
    float f_ab_v = edge_function(v, v_a, v_b);
    
    float f_ac_v_b = edge_function(v_b, v_a, v_c);
    float f_ac_v = edge_function(v, v_a, v_c);
    
    float gamma = f_ab_v / f_ab_v_c;
    float beta = f_ac_v / f_ac_v_b;
    float alpha = 1 - gamma - beta;

    return Vec3f(alpha, beta, gamma);
}

inline
Vec3f compute_barycentric_coord_2D(const Vec2f& v, const Vec2f& v_a, const Vec2f& v_b, const Vec2f& v_c, float f_ac_vb, float f_ab_vc)
{
    float f_ab_v = edge_function(v, v_a, v_b);
    float f_ac_v = edge_function(v, v_a, v_c);

    float gamma = f_ab_v / f_ab_vc;
    float beta = f_ac_v / f_ac_vb;
    float alpha = 1 - gamma - beta;

    return Vec3f(alpha, beta, gamma);
}
inline
Vec2f interpolate(const Vec2f& v0, const Vec2f& v1, const Vec2f& v2, float alpha, float beta, float gamma)
{
    return Vec2f(
        v0.x_ * alpha + v1.x_ * beta + v2.x_ * gamma,
        v0.y_ * alpha + v1.y_ * beta + v2.y_ * gamma);
}

inline
Vec3f interpolate(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, float alpha, float beta, float gamma)
{
    return Vec3f(
        v0.x_ * alpha + v1.x_ * beta + v2.x_ * gamma,
        v0.y_ * alpha + v1.y_ * beta + v2.y_ * gamma,
        v0.z_ * alpha + v1.z_ * beta + v2.z_ * gamma);
}

inline
Vec4f interpolate(const Vec4f& v0, const Vec4f& v1, const Vec4f& v2, float alpha, float beta, float gamma)
{
    return Vec4f(
        v0.x_ * alpha + v1.x_ * beta + v2.x_ * gamma,
        v0.y_ * alpha + v1.y_ * beta + v2.y_ * gamma,
        v0.z_ * alpha + v1.z_ * beta + v2.z_ * gamma,
        1.0f);
}

#endif