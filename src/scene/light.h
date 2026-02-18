#ifndef __LIGHT_H__
#define __LIGHT_H__

enum LightType
{
    DIRECTIONAL,
    POINT,
    SPOT
};

struct Light
{
    LightType type;
    Vec3f color;
    float intensity;

    virtual LightType get_type() const { return type; }
    virtual void print_info() const = 0;
    virtual ~Light() = default;
};

struct DirectionalLight : public Light
{
    Vec3f direction;

    void print_info() const override
    {
        std::cout << "LightType: " << type << std::endl;
        std::cout << "Color: " << color << std::endl;
        std::cout << "Intensity: " << intensity << std::endl;
        
        std::cout << "Direction: " << direction << std::endl;

        return;
    }
};

struct PointLight : public Light
{

};

struct SpotLight : public Light
{

};

#endif