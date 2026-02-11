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

void Renderer::render(const Scene& scene)
{
    update_per_frame_uniform(scene);

    ShaderContext shader_context;
    // set Light Uniform

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

    const std::vector<SubMesh>& sub_meshes = model.get_sub_meshes();
    for(auto it = sub_meshes.begin(); it != sub_meshes.end(); ++it)
    {
        const SubMesh& sub_mesh = *it;
        update_per_sub_mesh_uniform(sub_mesh);
        shader_context.set_uniform(&uniform_);

        std::shared_ptr<Material> spMaterial = sub_mesh.wpMaterial.lock();
        if (spMaterial)
        {
            shader_context.material = spMaterial.get();
            std::shared_ptr<Shader> spShader = spMaterial->wpShader.lock();
            if(spShader)
            {
                draw_call(mesh, sub_mesh.index_offset, sub_mesh.index_count, spShader.get(), shader_context);
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
    uniform_.projection_matrix = perspective(camera.get_fovy(), camera.get_aspect(), -camera.get_near_plane(), -camera.get_far_plane());
    uniform_.VP_matrix = uniform_.projection_matrix * uniform_.view_matrix;
    uniform_.world_camera_position = camera.get_pos();
}

void Renderer::update_per_model_uniform(const Transform& t)
{
    uniform_.model_matrix = transform(uniform_.model_matrix, t);
    uniform_.MVP_matrix = uniform_.VP_matrix * uniform_.model_matrix;
}

void Renderer::update_per_sub_mesh_uniform(const SubMesh& sub_mesh)
{
    uniform_.sub_mesh_matrix = Matrix::Identity;
    //uniform_.MVP_matrix = uniform_.sub_mesh_matrix * uniform_.MVP_matrix;
}