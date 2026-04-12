#include "scene/material.h"

Material::Material(const Material& mat)
    : type(mat.type), name(mat.name), wpShader(mat.wpShader) {}

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
    std::cout << "type: " << type << std::endl;
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

PBRMaterial::PBRMaterial(const PBRMaterial& mat)
    : Material(mat),
    albedo(mat.albedo),
    roughness(mat.roughness),
    metallic(mat.metallic),
    wpAlbedo_map(mat.wpAlbedo_map), 
    wpRoughness_map(mat.wpRoughness_map), 
    wpMetallic_map(mat.wpMetallic_map),
    wpNormal_map(mat.wpNormal_map),
    wpAO_map(mat.wpAO_map) {}

void PBRMaterial::print_info() const
{
    std::cout << "type: " << type << std::endl;
    std::cout << "name: " << name << std::endl;
    std::cout << "shader: ";
    if (!wpShader.expired())
        std::cout << wpShader.lock()->get_name() << std::endl;
    else std::cout << "null" << std::endl;

    std::cout << "albedo: " << albedo << std::endl; 
    std::cout << "roughness: " << roughness << std::endl;
    std::cout << "metallic: " << metallic << std::endl;

    std::cout << "albedo_map: ";
    if (auto tex = wpAlbedo_map.lock())
        std::cout << tex->get_name() << std::endl;
    else std::cout << "null" << std::endl;
    
    std::cout << "roughness_map: ";
    if (auto tex = wpRoughness_map.lock())
        std::cout << tex->get_name() << std::endl;
    else std::cout << "null" << std::endl;
    
    std::cout << "metallic_map: ";
    if (auto tex = wpMetallic_map.lock())
        std::cout << tex->get_name() << std::endl;
    else std::cout << "null" << std::endl;

    std::cout << "normal_map: ";
    if (auto tex = wpNormal_map.lock())
        std::cout << tex->get_name() << std::endl;
    else std::cout << "null" << std::endl;
    
    std::cout << "ao_map: ";
    if (auto tex = wpAO_map.lock())
        std::cout << tex->get_name() << std::endl;
    else std::cout << "null" << std::endl;
}