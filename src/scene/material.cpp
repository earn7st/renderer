#include "scene/material.h"

Material::Material(const Material& mat)
    : name(mat.name), wpShader(mat.wpShader) {}

BlinnPhongMaterial::BlinnPhongMaterial(const BlinnPhongMaterial& mat)
    : Material(mat), 
    ambient(mat.ambient), 
    diffuse(mat.diffuse), 
    specular(mat.specular), 
    shininess(mat.shininess), 
    optical_density(mat.optical_density), 
    transparency(mat.transparency), 
    illumination_model(mat.illumination_model), 
    wpDiffuse_map(mat.wpDiffuse_map), 
    wpSpecular_map(mat.wpSpecular_map), 
    wpBump_map(mat.wpBump_map),
    wpAlpha_map(mat.wpAlpha_map) {}

void BlinnPhongMaterial::print_info() const
{
    std::cout << "name: " << name << std::endl;
    std::cout << "shader: ";
    if (!wpShader.expired())
        std::cout << wpShader.lock()->get_name() << std::endl;
    else std::cout << "null" << std::endl;
    std::cout << "ambient: " << ambient << std::endl; 
    std::cout << "diffuse: " << diffuse << std::endl;
    std::cout << "specular: " << specular << std::endl;
    std::cout << "shininess: " << shininess << std::endl;
    std::cout << "optical_density: " << optical_density << std::endl;
    std::cout << "transparency: " << transparency << std::endl;

    std::cout << "diffuse_map: ";
    if (auto tex = wpDiffuse_map.lock())
        std::cout << tex->get_name() << std::endl;
    else std::cout << "null" << std::endl;
    
    std::cout << "specular_map: ";
    if (auto tex = wpSpecular_map.lock())
        std::cout << tex->get_name() << std::endl;
    else std::cout << "null" << std::endl;
    
    std::cout << "bump_map: ";
    if (auto tex = wpBump_map.lock())
        std::cout << tex->get_name() << std::endl;
    else std::cout << "null" << std::endl;
    
    std::cout << "alpha_map: ";
    if (auto tex = wpAlpha_map.lock())
        std::cout << tex->get_name() << std::endl;
    else std::cout << "null" << std::endl;
}