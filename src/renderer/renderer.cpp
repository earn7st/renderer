#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "scene/transform.h"
#include "utils.hpp"

uint32_t cnt = 0;

Renderer::Renderer(RenderState rs)
{
    render_state_.polygon_mode = rs.polygon_mode;
}

bool Renderer::attach_framebuffer(Framebuffer* fb)
{
    if(fb->is_valid())
    {
        fb_ = fb;
        return true;
    }
    return false;
}

void Renderer::set_render_state(const RenderState& render_state)
{
    render_state_ = render_state;
}

void Renderer::initialize_lights(const Scene& scene)
{
    const std::vector<std::shared_ptr<Light>>& lights = scene.get_lights();
    for (int i = 0; i < lights.size(); ++i)
    {
        const std::shared_ptr<Light>& light = lights[i];
        if (light->get_type() == DIRECTIONAL)
        {
            auto light_ptr = std::dynamic_pointer_cast<DirectionalLight>(light);
            if (light_ptr)
            {
                light_uniform_.directional_lights.push_back(*light_ptr);
            }
        } else if (light->get_type() == POINT)
        {

        } else if (light->get_type() == SPOT)
        {

        }
    }
}

void Renderer::render(const Scene& scene)
{
    update_per_frame_uniform(scene);

    ShaderContext shader_context;
    shader_context.set_light_uniform(&light_uniform_);

    const std::vector<Model>& models = scene.get_models();
    for(auto it = models.begin(); it != models.end(); ++it)
    {
        const Model& model = *it;
        draw_model(model, shader_context);
    }
    
}

void Renderer::draw_model(const Model& model, ShaderContext& shader_context)
{
    const Transform& transform = model.get_transform();
    update_per_model_uniform(transform);

    const std::weak_ptr wpMesh = model.mesh_weak();
    const std::shared_ptr spMesh = wpMesh.lock();

    const Mesh& mesh = *spMesh;

    const std::vector<SubMesh>& submeshes = model.get_submeshes();
    for(auto it = submeshes.begin(); it != submeshes.end(); ++it)
    {
        const SubMesh& submesh = *it;
        update_per_submesh_uniform(submesh);
        shader_context.set_uniform(&uniform_);

        std::shared_ptr<Material> spMaterial = submesh.wpMaterial.lock();
        if (spMaterial)
        {
            shader_context.material = spMaterial.get();
            std::shared_ptr<Shader> spShader = spMaterial->wpShader.lock();
            if(spShader)
            {
                draw_call(mesh, submesh.index_offset, submesh.index_count, spShader.get(), shader_context);
            }
        }
    }

}

void Renderer::draw_call(const Mesh& mesh, uint32_t offset, uint32_t count, const Shader* shader, const ShaderContext& shader_context)
{
    if(count % 3 != 0){
        std::cerr << "Renderer::draw_call(): Size cannot divided by 3!\n";
        return ;
    }

    for (uint32_t i = offset; i < offset + count; i += 3)
    {
        const Vertex& ori_v0 = mesh.vertices[mesh.indices[i]];
        const Vertex& ori_v1 = mesh.vertices[mesh.indices[i + 1]];
        const Vertex& ori_v2 = mesh.vertices[mesh.indices[i + 2]];

        Varying v0 = shader->execute_vertex_shader(ori_v0, shader_context);
        Varying v1 = shader->execute_vertex_shader(ori_v1, shader_context);
        Varying v2 = shader->execute_vertex_shader(ori_v2, shader_context);

        // (Optional) Geometry Shading
        // (Optional) Culling
        // (Optional) Clipping

        // Rasterization : Perspective Division, Screen Mapping, rasterizing
        rasterizer_.rasterize(v0, v1, v2, fb_, shader, shader_context, render_state_);
    }
}

void Renderer::update_per_frame_uniform(const Scene& scene)
{
    const Camera& camera = scene.get_main_camera();
    uniform_.view_matrix = lookAt(camera.get_pos(), camera.get_center(), camera.get_up());
    uniform_.projection_matrix = perspective(camera.get_fovy(), camera.get_aspect(), camera.get_near_plane(), camera.get_far_plane());
    uniform_.VP_matrix = uniform_.projection_matrix * uniform_.view_matrix;
    uniform_.world_camera_position = camera.get_pos();
}

void Renderer::update_per_model_uniform(const Transform& t)
{
    uniform_.model_matrix = transform(Matrix::Identity, t);
}

void Renderer::update_per_submesh_uniform(const SubMesh& submesh)
{
    uniform_.submesh_matrix = transform(Matrix::Identity, submesh.local_transform);

    Matrix combined_matrix = uniform_.model_matrix * uniform_.submesh_matrix;
    uniform_.normal_matrix = get_normal_matrix(combined_matrix);
    uniform_.MVP_matrix = uniform_.VP_matrix * combined_matrix;
}