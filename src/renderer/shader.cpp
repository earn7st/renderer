#include "math/vector.hpp"
#include "math/geometry.hpp"
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
    out.tangent = uniform.normal_matrix * input.tangent;
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

RGBA PBR_fragment_shader(const FragmentIn& input, const ShaderConstants& shader_constants)
{
    const Uniform& uniform = shader_constants.uniform;
    const LightUniform& light_uniform = shader_constants.light_uniform;
    const MaterialData& mat_data = shader_constants.mat_data;

    Vec3f world_pos = Vec3f(input.world_pos.x_, input.world_pos.y_, input.world_pos.z_);
    Vec3f N = normalize(Vec3f(input.world_normal.x_, input.world_normal.y_, input.world_normal.z_));
    Vec3f V = normalize(uniform.world_camera_position - world_pos);

    // --- Normal mapping ---
    if (mat_data.pNormal_map)
    {
        Vec3f T(input.tangent.x_, input.tangent.y_, input.tangent.z_);
        T = normalize(T);
        float handedness = input.tangent.w_ >= 0.0f ? 1.0f : -1.0f;

        // Re-orthogonalise T against N (Gram-Schmidt)
        T = normalize(T - N * dot(N, T));
        Vec3f B = cross(N, T) * handedness;

        // Sample normal map and decode [0,1] → [-1,1]
        Vec3f tn = mat_data.pNormal_map->textureRGB(input.texcoord.x_, input.texcoord.y_);
        Vec3f ts_normal(tn.x_ * 2.0f - 1.0f,
                        tn.y_ * 2.0f - 1.0f,
                        tn.z_ * 2.0f - 1.0f);
        ts_normal = normalize(ts_normal);

        // Transform tangent-space normal to world space
        N = normalize(T * ts_normal.x_ + B * ts_normal.y_ + N * ts_normal.z_);
    }

    // Sampling
    // Albedo, To Linear
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
    
    // F0
    // 0.04 for dielectric，Albedo color for metal
    Vec3f F0 = Vec3f(0.04f); 
    F0 = F0 * (1.0f - metallic) + albedo * metallic; // lerp

    Vec3f Lo(0.0f);

    // Calculate Direct Lighting
    for (const auto& light : light_uniform.directional_lights) {
        Vec3f L = normalize(-light.direction);
        Vec3f H = normalize(V + L);
        
        // radiance
        Vec3f radiance = light.color * light.intensity;

        // Cook-Torrance BRDF
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        Vec3f F = fresnelSchlick(std::max(dot(H, V), 0.0f), F0);

        // specular
        Vec3f numerator = F * D * G;
        float denominator = 4.0f * std::max(dot(N, V), 0.0f) * std::max(dot(N, L), 0.0f) + 0.0001f; // 防止除 0
        Vec3f specular = numerator / denominator;

        // diffuse
        Vec3f kS = F;
        Vec3f kD = Vec3f(1.0f) - kS;
        kD *= 1.0f - metallic;

        float NdotL = std::max(dot(N, L), 0.0f);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // ambient
    Vec3f ambient = Vec3f(0.03f) * albedo * ao; // Simplfied ambient light
    Vec3f color = ambient + Lo;

    // Simple Tone Mapping
    color = color / (color + Vec3f(1.0f));

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