#include "math/vector.hpp"
#include "renderer/shader.h"
#include "renderer/render_types.hpp"

VertexOut Shader::execute_vertex_shader(const Vertex& input, const ShaderConstants& shader_constants) const
{
    return vertex_shader_(input, shader_constants);
}

RGBA Shader::execute_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants) const
{
    return fragment_shader_(input, shader_constants);
}

VertexOut standard_vertex_shader(const Vertex& input, const ShaderConstants& shader_constants)
{
    VertexOut out;
    const Uniform& uniform = shader_constants.uniform;
    out.clip_pos = uniform.MVP_matrix * input.pos;
    out.clip_w = out.clip_pos.w_;
    out.world_pos = uniform.model_matrix * input.pos;
    out.world_normal = uniform.normal_matrix * input.normal; 
    out.texcoord = input.texcoord;

    return out;
}

RGBA blinn_phong_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    // Get References of Uniform
    const Uniform& uniform = shader_constants.uniform;
    const LightUniform& light_uniform = shader_constants.light_uniform;
    const MaterialData& mat_data = shader_constants.mat_data;

    // Preparation
    Vec3f frag_pos = Vec3f(input.world_pos.x_, input.world_pos.y_, input.world_pos.z_);
    Vec3f camera_pos = uniform.world_camera_position; 
    Vec3f view_dir = normalize(camera_pos - frag_pos); 
    Vec3f normal = normalize(Vec3f(input.world_normal.x_, input.world_normal.y_, input.world_normal.z_));

    Vec3f ambient = Vec3f(mat_data.ambient.x_, mat_data.ambient.y_, mat_data.ambient.z_) * light_uniform.ambient_light_color * light_uniform.ambient_intensity;
    Vec3f diffuse_total(0.f);
    Vec3f specular_total(0.f);

    // Textures
    const Texture* diffuse_tex = mat_data.pDiffuse_map;
    const Texture* specular_tex = mat_data.pSpecular_map;
    const Texture* bump_tex = mat_data.pBump_map;
    const Texture* alpha_tex = mat_data.pAlpha_map;

    // Directional Lights
    for (const auto& light:light_uniform.directional_lights)
    {
        Vec3f light_dir = normalize(-light.direction);
        
        // Diffuse
        Vec3f albedo(0.f);
        float diff = std::max(dot(normal, light_dir), 0.f);
        if (diffuse_tex)
        {
            Vec3f diffuse_tex_color = diffuse_tex->textureRGB(input.texcoord.x_, input.texcoord.y_);
            albedo = diffuse_tex_color * Vec3f(mat_data.diffuse.x_, mat_data.diffuse.y_, mat_data.diffuse.z_);
        } else 
        {
            albedo = Vec3f(mat_data.diffuse.x_, mat_data.diffuse.y_, mat_data.diffuse.z_); 
        }
        diffuse_total += albedo * light.color * diff * light.intensity;

        // Specular
        Vec3f halfway_dir = normalize(view_dir + light_dir);
        float spec = std::pow(std::max(dot(normal, halfway_dir), 0.0f), mat_data.shininess);
        specular_total += Vec3f(mat_data.specular.x_, mat_data.specular.y_, mat_data.specular.z_) * light.color * spec;
    }
    
    Vec3f final_color = ambient + diffuse_total + specular_total;

    final_color.x_ = std::min(1.f, final_color.x_);
    final_color.y_ = std::min(1.f, final_color.y_);
    final_color.z_ = std::min(1.f, final_color.z_);

    return RGBA(final_color, 1.f);
}
// 辅助函数：正态分布函数 (Trowbridge-Reitz GGX)
float DistributionGGX(Vec3f N, Vec3f H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = std::max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = 3.14159265359f * denom * denom;
    return nom / denom;
}

// 辅助函数：几何遮蔽函数 (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;
    return nom / denom;
}

float GeometrySmith(Vec3f N, Vec3f V, Vec3f L, float roughness) {
    float NdotV = std::max(dot(N, V), 0.0f);
    float NdotL = std::max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// 辅助函数：菲涅尔方程 (Fresnel-Schlick)
Vec3f fresnelSchlick(float cosTheta, Vec3f F0) {
    return F0 + (Vec3f(1.0f) - F0) * std::pow(std::clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

RGBA PBR_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    const Uniform& uniform = shader_constants.uniform;
    const LightUniform& light_uniform = shader_constants.light_uniform;
    const MaterialData& mat_data = shader_constants.mat_data;

    // 1. 准备基础向量
    Vec3f world_pos = Vec3f(input.world_pos.x_, input.world_pos.y_, input.world_pos.z_);
    Vec3f N = normalize(Vec3f(input.world_normal.x_, input.world_normal.y_, input.world_normal.z_));
    Vec3f V = normalize(uniform.world_camera_position - world_pos);

    // 2. 采样贴图并处理空间
    // Albedo 转线性空间
    Vec3f albedo;
    if (mat_data.pAlbedo_map) {
        Vec3f srgb = mat_data.pAlbedo_map->textureRGB(input.texcoord.x_, input.texcoord.y_);
        albedo = Vec3f(std::pow(srgb.x_, 2.2f), std::pow(srgb.y_, 2.2f), std::pow(srgb.z_, 2.2f));
    } else {
        albedo = Vec3f(mat_data.albedo.x_, mat_data.albedo.y_, mat_data.albedo.z_);
    }

    float metallic  = mat_data.pMetallic_map  ? mat_data.pMetallic_map->textureRGB(input.texcoord.x_, input.texcoord.y_).x_ : mat_data.metallic;
    float roughness = mat_data.pRoughness_map ? mat_data.pRoughness_map->textureRGB(input.texcoord.x_, input.texcoord.y_).x_ : mat_data.roughness;
    float ao        = mat_data.pAO_map        ? mat_data.pAO_map->textureRGB(input.texcoord.x_, input.texcoord.y_).x_ : 1.0f;

    // 3. 计算 F0 (基础反射率)
    // 非金属固定为 0.04，金属则使用 Albedo 颜色
    Vec3f F0 = Vec3f(0.04f); 
    F0 = F0 * (1.0f - metallic) + albedo * metallic; // 假设你有一个 lerp 函数，或者手动：F0 * (1-m) + albedo * m

    Vec3f Lo(0.0f);

    // 4. 计算直接光照 (这里以 Directional Light 为例)
    for (const auto& light : light_uniform.directional_lights) {
        Vec3f L = normalize(-light.direction);
        Vec3f H = normalize(V + L);
        
        // 辐射率 (辐射强度)
        Vec3f radiance = light.color * light.intensity;

        // Cook-Torrance BRDF
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        Vec3f F = fresnelSchlick(std::max(dot(H, V), 0.0f), F0);

        // 计算镜面反射部分
        Vec3f numerator = F * D * G;
        float denominator = 4.0f * std::max(dot(N, V), 0.0f) * std::max(dot(N, L), 0.0f) + 0.0001f; // 防止除 0
        Vec3f specular = numerator / denominator;

        // 计算漫反射部分 (能量守恒)
        Vec3f kS = F;
        Vec3f kD = Vec3f(1.0f) - kS;
        kD *= 1.0f - metallic; // 金属不产生漫反射

        float NdotL = std::max(dot(N, L), 0.0f);
        Lo += (kD * albedo / 3.14159265359f + specular) * radiance * NdotL;
    }

    // 5. 环境光与最终整合
    Vec3f ambient = Vec3f(0.03f) * albedo * ao; // 简化的环境光
    Vec3f color = ambient + Lo;


    // color = color / (color + Vec3f(1.0f));
    color = Vec3f(std::pow(color.x_, 1.0f/2.2f), std::pow(color.y_, 1.0f/2.2f), std::pow(color.z_, 1.0f/2.2f));

    return RGBA(color, 1.0f);
}

RGBA flat_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    return RGBA(0.f, 0.f, 1.f, 1.f);
}

RGBA wireframe_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    return RGBA(0.f, 0.f, 0.f, 1.f); // Black
}