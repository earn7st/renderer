#ifndef __LIGHT_H__
#define __LIGHT_H__

struct Light
{
    Vec3f color;
    float intensity;
};

struct DirectionalLight : public Light
{
    Vec3f direction;
};

struct PointLight : public Light
{

};

struct SpotLight : public Light
{

};

#endif