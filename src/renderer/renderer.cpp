#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "scene/transform.h"
#include "utils.hpp"

uint32_t cnt = 0;

bool Renderer::attach_framebuffer(Framebuffer& fb)
{
    if(fb.is_valid())
    {
        fb_ = &fb;
        return true;
    }
    return false;
}

void Renderer::render(const Scene& scene)
{
    update_per_frame_uniform(scene);

    const std::vector<Model>& models = scene.get_models();
    for(auto it = models.begin(); it != models.end(); ++it)
    {
        const Model& model = *it;
        draw_model(model);
    }
    
}

void Renderer::draw_model(const Model& model)
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

        // TODO: Material
        // const Material* pMaterial = model.get_pMaterial(sub_mesh.material_index);
        
        // TODO: set per-sub_mesh material info to uniform;
        // uniform_ = update_uniform_material()...

        draw_call(mesh, sub_mesh.offset, sub_mesh.size);
    }

}

void Renderer::draw_call(const Mesh& mesh, uint32_t offset, uint32_t size)
{
    if(size % 3 != 0){
        std::cerr << "Renderer::draw_call(): Size cannot divided by 3!\n";
        return ;
    }
    for (uint32_t i = offset; i < offset + size; i += 3)
    {
        const Vertex& ori_v0 = mesh.vertices[mesh.indices[i]];
        const Vertex& ori_v1 = mesh.vertices[mesh.indices[i + 1]];
        const Vertex& ori_v2 = mesh.vertices[mesh.indices[i + 2]];

        Varying v0 = shader_.execute_vertex_shader(ori_v0, uniform_);
        Varying v1 = shader_.execute_vertex_shader(ori_v1, uniform_);
        Varying v2 = shader_.execute_vertex_shader(ori_v2, uniform_);

        // (Optional) Geometry Shading
        // (Optional) Culling
        // (Optional) Clipping

        // Rasterization : Perspective Division, Screen Mapping, rasterizing
        rasterizer_.rasterize(v0, v1, v2, fb_, shader_, uniform_, render_state_);

    }
}

void Renderer::update_per_frame_uniform(const Scene& scene)
{
    const Camera& camera = scene.get_main_camera();

    uniform_.view_matrix = lookAt(camera.get_pos(), camera.get_center(), camera.get_up());
    uniform_.projection_matrix = perspective(camera.get_fovy(), camera.get_aspect(), -camera.get_near_plane(), -camera.get_far_plane());
    uniform_.VP_matrix = uniform_.projection_matrix * uniform_.view_matrix;
    uniform_.world_camera_position = camera.get_pos();

    // TODO: lighting

}

void Renderer::update_per_model_uniform(const Transform& t)
{
    uniform_.model_matrix = transform(uniform_.model_matrix, t);
    uniform_.MVP_matrix = uniform_.VP_matrix * uniform_.model_matrix;
}

void Renderer::update_per_sub_mesh_uniform(const SubMesh& sub_mesh)
{
    // TODO
    uniform_.sub_mesh_matrix = Matrix::Identity;
    //uniform_.MVP_matrix = uniform_.sub_mesh_matrix * uniform_.MVP_matrix;
}