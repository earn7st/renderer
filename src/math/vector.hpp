#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <iostream>
#include <cmath>
#include <iomanip>

#ifdef _WIN32
    #include <cstdint>
#endif

// Vector2
template <typename T>
class Vector2
{
public:
    T x_, y_;

    Vector2<T>(T x = 0, T y = 0) : x_(x), y_(y) {}
    Vector2<T>(const Vector2<T>& v) : x_(v.x_), y_(v.y_) {}
    Vector2<T>& operator = (const Vector2<T>& v)
    {
        if(this == &v) return *this;
        x_ = v.x_, y_ = v.y_;
        return *this;
    }
    
    bool operator == (const Vector2<T>& v) const { return (x_ == v.x_ && y_ == v.y_); }
    bool operator != (const Vector2<T>& v) const { return (x_ != v.x_ || y_ != v.y_); }

    Vector2<T> operator + (const Vector2<T>& v) const { return Vector2<T>(x_ + v.x_, y_ + v.y_); }
    Vector2<T>& operator += (const Vector2<T>& v) { x_ += v.x_, y_ += v.y_; return *this; }
    Vector2<T> operator - (const Vector2<T>& v) const { return Vector2<T>(x_ - v.x_, y_ - v.y_); }
    Vector2<T>& operator -= (const Vector2<T>& v) { x_ -= v.x_, y_ -= v.y_; return *this; }
    template <typename U>
    Vector2<T> operator / (const U d) const { return Vector2<T>(x_ / d, y_ / d);}
    template <typename U>
    Vector2<T>& operator /= (const U d) { x_ /= d, y_ /= d; return *this; }
    template <typename U>
    Vector2<T> operator * (const U d) const { return Vector2<T>(x_ * d, y_ * d);}
    template <typename U>
    Vector2<T>& operator *= (const U d) { x_ *= d, y_ *= d; return *this; }

    double length2() const { return x_ * x_ + y_ * y_; }
    double length() const { return sqrt(length2()); }
    void normalize() { double len = this->length(); x_ /= len, y_ /= len; }

};

template <typename T>
inline 
Vector2<T> normalized(const Vector2<T>& v)
{
    double len = v.length();
    return v / len;
}

template <typename T>
inline 
T dot(const Vector2<T>& v1, const Vector2<T>& v2) { return v1.x_ * v2.x_ + v1.y_ * v2.y_; }

template <typename T>
inline 
T cross(const Vector2<T>& v1, const Vector2<T>& v2) { return v1.x_ * v2.y_ - v1.y_ * v2.x_;}

template <typename T>
inline 
std::ostream& operator << (std::ostream& os, Vector2<T> v)
{ 
    return os << "(" << std::setw(4) << v.x_ << ", " << std::setw(4) << v.y_ << ")" << std::endl;
}

typedef Vector2<int> Vec2i;
typedef Vector2<float> Vec2f;
typedef Vector2<double> Vec2d;
typedef Vector2<float> Coord2f;
typedef Vector2<double> Coord2d;


// Vector3
template <typename T>
class Vector3
{
public:
    T x_, y_, z_;

    Vector3<T>() : Vector3(0, 0, 0) {}
    Vector3<T>(T x, T y, T z) : x_(x), y_(y), z_(z) {}
    Vector3<T>(T value) : x_(value), y_(value), z_(value) {}
    Vector3<T>(const Vector3<T>& v) : x_(v.x_), y_(v.y_), z_(v.z_) {}
    Vector3<T>& operator = (const Vector3<T>& v)
    {
        if(this == &v) return *this;
        x_ = v.x_, y_ = v.y_, z_ = v.z_;
        return *this;
    }

    bool operator == (const Vector3<T>& v) const { return (x_ == v.x_ && y_ == v.y_ && z_ == v.z_); }
    bool operator != (const Vector3<T>& v) const { return (x_ != v.x_ || y_ != v.y_ || z_ != v.z_); }

    Vector3<T> operator + (const Vector3<T>& v) const { return Vector3<T>(x_ + v.x_, y_ + v.y_, z_ + v.z_); }
    Vector3<T>& operator += (const Vector3<T>& v) {  x_ += v.x_, y_ += v.y_, z_ += v.z_; return *this; }
    Vector3<T> operator - (const Vector3<T>& v) const { return Vector3<T>(x_ - v.x_, y_ - v.y_, z_ - v.z_); }
    Vector3<T> operator - () const { return Vector3<T>(-x_, -y_, -z_); }
    Vector3<T>& operator -= (const Vector3<T>& v) {  x_ -= v.x_, y_ -= v.y_, z_ -= v.z_; return *this; }
    Vector3<T> operator * (const Vector3<T>& v) const { return Vector3<T>(x_ * v.x_, y_ * v.y_, z_ * v.z_); }
    Vector3<T> operator * (const T& v) const { return Vector3<T>(x_ * v, y_ * v, z_ * v); }
    Vector3<T>& operator *= (const T& v) { x_ *= v, y_ *= v, z_ *= v; return *this; }
    Vector3<T> operator / (const T& v) const { return Vector3<T>(x_ / v, y_ / v, z_ / v); }
    Vector3<T>& operator /= (const T& v) { x_ /= v, y_ /= v, z_ /= v; return *this; }

    double length2() const { return x_ * x_ + y_ * y_ + z_ * z_; }
    double length() const { return sqrt(length2()); }

};

template <typename T>
inline
Vector3<T> normalize(const Vector3<T>& v)
{
    double len = v.length();
    return v / len;
}

template <typename T>
inline
T dot(const Vector3<T>& v1, const Vector3<T>& v2) { return v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_; }

template <typename T>
inline
Vector3<T> cross(const Vector3<T>& v1, const Vector3<T>& v2)
{
    return Vector3<T>(
        v1.y_ * v2.z_ - v1.z_ * v2.y_,
        v1.z_ * v2.x_ - v1.x_ * v2.z_,
        v1.x_ * v2.y_ - v1.y_ * v2.x_
    );
}

template <typename T>
inline 
std::ostream& operator << (std::ostream& os, Vector3<T> v)
{ 
    return os << "(" << std::setw(4) << v.x_ << ", " << std::setw(4) << v.y_ << ", " << std::setw(4) << v.z_ << ")";
}

typedef Vector3<int> Vec3i;
typedef Vector3<unsigned int> Vec3u;
typedef Vector3<float> Vec3f;
typedef Vector3<double> Vec3d;
typedef Vector3<float> Pos3f;
typedef Vector3<double> Pos3d;
typedef Vector3<uint8_t> RGB;

template <typename T>
class Vector4
{
public:
    T x_, y_, z_, w_;

    Vector4<T>(T x = 0, T y = 0, T z = 0, T w = 0) : x_(x), y_(y), z_(z), w_(w) {}
    Vector4<T>(const Vector3<T>& v, T w) : x_(v.x_), y_(v.y_), z_(v.z_), w_(w) {}
    Vector4<T>(const Vector4<T>& v) : x_(v.x_), y_(v.y_), z_(v.z_), w_(v.w_) {}
    Vector4<T>& operator = (const Vector4<T>& v)
    {
        if(this == &v) return *this;
        x_ = v.x_, y_ = v.y_, z_ = v.z_, w_ = v.w_;
        return *this;
    }

    bool operator == (const Vector3<T>& v) const { return (x_ == v.x_ && y_ == v.y_ && z_ == v.z_ && w_ == v.w_); }
    bool operator != (const Vector3<T>& v) const { return (x_ != v.x_ || y_ != v.y_ || z_ != v.z_ || w_ != v.w_); }

    Vector4<T> operator + (const Vector4<T>& v) { return Vector4<T>(x_ + v.x_, y_ + v.y_, z_ + v.z_, w_ + v.w_); }
    Vector4<T>& operator += (const Vector4<T>& v) { x_ += v.x_, y_ += v.y_, z_ += v.z_, w_ += v.w_; return *this; }

    Vector4<T> operator / (const T& v) const { return Vector4<T>(x_/v, y_/v, z_/v, w_/v); }

};

template <typename T>
inline
std::ostream& operator << (std::ostream& os, Vector4<T> v)
{ 
    return os << "(" << std::setw(4) << v.x_ << ", " << std::setw(4) << v.y_ << ", " << std::setw(4) << v.z_ << ", " << std::setw(4) << v.w_ << ")";
}

typedef Vector4<int> Vec4i;
typedef Vector4<unsigned int> Vec4u;
typedef Vector4<float> Vec4f;
typedef Vector4<float> RGBA;
typedef Vector4<double> Vec4d;

#endif
