#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "scene/transform.h"

uint32_t cnt = 0;

bool should_clip(const Varying &v) {
    float w = v.clip_w;
    return (v.clip_pos.x_ > w || v.clip_pos.x_ < -w) ||
           (v.clip_pos.y_ > w || v.clip_pos.y_ < -w) ||
           (v.clip_pos.z_ > w || v.clip_pos.z_ < -w);
}

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

void Renderer::render(const Scene& scene, const FpsCamera& camera)
{
    ShaderConstants shader_constants;
    shader_constants.light_uniform = light_uniform_;

    update_per_frame_uniform(camera);

    const std::vector<Model>& models = scene.get_models();
    for(auto it = models.begin(); it != models.end(); ++it)
    {
        const Model& model = *it;
        draw_model(model, shader_constants);
    }
}

void Renderer::render_parallel(const Scene& scene, const FpsCamera& camera)
{
    ShaderConstants shader_constants;
    shader_constants.light_uniform = light_uniform_;

    update_per_frame_uniform(camera);

    uint32_t sw = fb_->get_width();
    uint32_t sh = fb_->get_height();

    // ---- Step 1: collect draw batches (serial, fast) ----
    std::vector<DrawBatch> batches;
    collect_draw_batches(scene, batches);
    if (batches.empty()) return;

    // Count total triangles and build prefix sum
    uint32_t total_tris = 0;
    std::vector<uint32_t> batch_start(batches.size());
    for (size_t bi = 0; bi < batches.size(); ++bi) {
        batch_start[bi] = total_tris;
        total_tris += batches[bi].triangle_count;
    }
    if (total_tris == 0) return;

    // Pre-allocate processed triangle storage
    std::vector<ProcessedTriangle> processed(total_tris);

    // ---- Step 2: parallel vertex processing (chunked) ----
    // Process triangles in chunks of 64 to amortise job-dispatch overhead.
    const int CHUNK = 64;
    int num_chunks = ((int)total_tris + CHUNK - 1) / CHUNK;

    job_system_.parallel_for(num_chunks, [&](int chunk_idx) {
        uint32_t start_tri = (uint32_t)chunk_idx * CHUNK;
        uint32_t end_tri   = std::min(start_tri + (uint32_t)CHUNK, total_tris);

        // Find which batch contains start_tri
        size_t bi = 0;
        while (bi < batches.size() && batch_start[bi] + batches[bi].triangle_count <= start_tri)
            ++bi;

        uint32_t local_offset = start_tri - batch_start[bi];  // first triangle within batch[bi]

        for (uint32_t tri_idx = start_tri; tri_idx < end_tri; ++tri_idx) {
            // Advance to next batch if needed
            while (bi < batches.size() && local_offset >= batches[bi].triangle_count) {
                local_offset = 0;
                ++bi;
            }
            if (bi >= batches.size()) break;

            const DrawBatch& b = batches[bi];
            uint32_t t = local_offset++;
            uint32_t i = b.first_index + t * 3;

            const Vertex& ori_v0 = b.mesh->vertices[b.mesh->indices[i]];
            const Vertex& ori_v1 = b.mesh->vertices[b.mesh->indices[i + 1]];
            const Vertex& ori_v2 = b.mesh->vertices[b.mesh->indices[i + 2]];

            Varying v0 = b.shader->execute_vertex_shader(ori_v0, b.constants);
            Varying v1 = b.shader->execute_vertex_shader(ori_v1, b.constants);
            Varying v2 = b.shader->execute_vertex_shader(ori_v2, b.constants);

            // Backface culling (world-space)
            if (render_state_.cull_mode == BACK) {
                Vec3f w0(v0.world_pos.x_, v0.world_pos.y_, v0.world_pos.z_);
                Vec3f w1(v1.world_pos.x_, v1.world_pos.y_, v1.world_pos.z_);
                Vec3f w2(v2.world_pos.x_, v2.world_pos.y_, v2.world_pos.z_);
                Vec3f face_normal = cross(w1 - w0, w2 - w0);
                Vec3f view_dir   = b.constants.uniform.world_camera_position - w0;
                if (dot(face_normal, view_dir) <= 0.0f)
                    continue;
            }

            if (should_clip(v0) && should_clip(v1) && should_clip(v2))
                continue;  // leave at default-constructed (shader=nullptr)

            ProcessedTriangle& pt = processed[tri_idx];
            pt.screen_pos[0] = rasterizer_.viewport_transform(v0.clip_pos / v0.clip_pos.w_, sw, sh);
            pt.screen_pos[1] = rasterizer_.viewport_transform(v1.clip_pos / v1.clip_pos.w_, sw, sh);
            pt.screen_pos[2] = rasterizer_.viewport_transform(v2.clip_pos / v2.clip_pos.w_, sw, sh);
            pt.ndc_pos[0] = v0.clip_pos / v0.clip_pos.w_;
            pt.ndc_pos[1] = v1.clip_pos / v1.clip_pos.w_;
            pt.ndc_pos[2] = v2.clip_pos / v2.clip_pos.w_;
            pt.varyings[0] = v0;
            pt.varyings[1] = v1;
            pt.varyings[2] = v2;
            pt.clip_w[0] = v0.clip_w;
            pt.clip_w[1] = v1.clip_w;
            pt.clip_w[2] = v2.clip_w;
            pt.shader = b.shader;
            pt.batch_idx = (uint32_t)bi;
        }
    });

    // ---- Step 3: serial binning into tiles ----
    // Binning is cheap (~5 ms for 11K tris) and serial execution avoids
    // per-frame mutex allocation + barrier overhead that caused progressive
    // slowdown in interactive mode.
    TileBinner binner(sw, sh);
    for (uint32_t i = 0; i < total_tris; ++i) {
        if (processed[i].shader == nullptr) continue;  // clipped
        binner.push_triangle(std::move(processed[i]));
    }

    // ---- Step 4: parallel tile rasterization ----
    int tile_count = binner.tile_count();
    job_system_.parallel_for(tile_count, [this, &binner, &batches](int tile_idx) {
        int tx = tile_idx % binner.tiles_x();
        int ty = tile_idx / binner.tiles_x();
        const TileBin& tile = binner.tile(tx, ty);
        if (!tile.triangle_indices.empty())
            rasterize_tile_job(tile, binner, batches);
    });
}

void Renderer::collect_draw_batches(const Scene& scene, std::vector<DrawBatch>& batches)
{
    ShaderConstants shader_constants;
    shader_constants.light_uniform = light_uniform_;

    const std::vector<Model>& models = scene.get_models();
    for (auto it = models.begin(); it != models.end(); ++it)
    {
        const Model& model = *it;
        const Transform& transform = model.get_transform();
        update_per_model_uniform(transform);

        const std::weak_ptr wpMesh = model.mesh_weak();
        const std::shared_ptr spMesh = wpMesh.lock();
        const Mesh& mesh = *spMesh;

        const std::vector<SubMesh>& submeshes = model.get_submeshes();
        for (auto sit = submeshes.begin(); sit != submeshes.end(); ++sit)
        {
            const SubMesh& submesh = *sit;
            update_per_submesh_uniform(submesh);
            shader_constants.uniform = uniform_;

            std::shared_ptr<Material> spMaterial = submesh.wpMaterial.lock();
            if (!spMaterial) continue;

            prepare_mat_data(shader_constants, spMaterial.get());
            std::shared_ptr<Shader> spShader = spMaterial->wpShader.lock();
            if (!spShader) continue;

            uint32_t count = submesh.index_count;
            if (count % 3 != 0) continue;

            DrawBatch batch;
            batch.mesh           = &mesh;
            batch.first_index    = submesh.index_offset;
            batch.triangle_count = count / 3;
            batch.shader         = spShader.get();
            batch.constants      = shader_constants;
            batches.push_back(std::move(batch));
        }
    }
}

void Renderer::rasterize_tile_job(const TileBin& tile, const TileBinner& binner,
                                   const std::vector<DrawBatch>& batches)
{
    for (uint32_t tri_idx : tile.triangle_indices)
    {
        const ProcessedTriangle& pt = binner.triangle(tri_idx);
        const DrawBatch& b = batches[pt.batch_idx];

        rasterizer_.rasterize_tile(
            pt.varyings[0], pt.varyings[1], pt.varyings[2],
            fb_, pt.shader, b.constants, render_state_,
            tile.min_x, tile.min_y, tile.max_x, tile.max_y);
    }
}

void Renderer::draw_model(const Model& model, ShaderConstants& shader_constants)
{
    const Transform& transform = model.get_transform();
    update_per_model_uniform(transform);

    const std::weak_ptr wpMesh = model.mesh_weak();
    const std::shared_ptr spMesh = wpMesh.lock();

    const Mesh& mesh = *spMesh;

    const std::vector<SubMesh>& submeshes = model.get_submeshes();
    for (auto it = submeshes.begin(); it != submeshes.end(); ++it)
    {
        const SubMesh& submesh = *it;
        update_per_submesh_uniform(submesh);
        shader_constants.uniform = uniform_;

        std::shared_ptr<Material> spMaterial = submesh.wpMaterial.lock();
        if (spMaterial)
        {
            prepare_mat_data(shader_constants, spMaterial.get());
            std::shared_ptr<Shader> spShader = spMaterial->wpShader.lock();
            if(spShader)
            {
                draw_call(mesh, submesh.index_offset, submesh.index_count, spShader.get(), shader_constants);
            }
        }
    }
    
}

void Renderer::draw_call(const Mesh& mesh, uint32_t offset, uint32_t count, const Shader* shader, const ShaderConstants& shader_constants)
{
    if (count % 3 != 0){
        std::cerr << "Renderer::draw_call(): Size cannot be divided by 3!\n";
        return ;
    }

    for (uint32_t i = offset; i < offset + count; i += 3)
    {
        const Vertex& ori_v0 = mesh.vertices[mesh.indices[i]];
        const Vertex& ori_v1 = mesh.vertices[mesh.indices[i + 1]];
        const Vertex& ori_v2 = mesh.vertices[mesh.indices[i + 2]];

        Varying v0 = shader->execute_vertex_shader(ori_v0, shader_constants);
        Varying v1 = shader->execute_vertex_shader(ori_v1, shader_constants);
        Varying v2 = shader->execute_vertex_shader(ori_v2, shader_constants);

        // Backface culling (world-space)
        if (render_state_.cull_mode == BACK)
        {
            Vec3f w0(v0.world_pos.x_, v0.world_pos.y_, v0.world_pos.z_);
            Vec3f w1(v1.world_pos.x_, v1.world_pos.y_, v1.world_pos.z_);
            Vec3f w2(v2.world_pos.x_, v2.world_pos.y_, v2.world_pos.z_);
            Vec3f face_normal = cross(w1 - w0, w2 - w0);
            Vec3f view_dir   = shader_constants.uniform.world_camera_position - w0;
            if (dot(face_normal, view_dir) <= 0.0f)
                continue;
        }

        // Clipping
        if (should_clip(v0) && should_clip(v1) && should_clip(v2))
        {
            continue;
        }

        // Rasterization : Perspective Division, Screen Mapping, rasterizing
        rasterizer_.rasterize(v0, v1, v2, fb_, shader, shader_constants, render_state_);
    }
}

void Renderer::update_per_frame_uniform(const FpsCamera& camera)
{
    uniform_.view_matrix = lookAt(camera.get_pos(), camera.get_center(), camera.get_up());
    uniform_.projection_matrix = perspective(camera.get_fovy(), camera.get_aspect(),
                                              camera.get_near_plane(), camera.get_far_plane());
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

void Renderer::prepare_mat_data(ShaderConstants& shader_constants, const Material* pMat) const
{
    MaterialData& mat_data = shader_constants.mat_data;
    if (pMat->get_type() == BLINN_PHONG_MAT)
    {
        const BlinnPhongMaterial& mat = *(static_cast<const BlinnPhongMaterial*>(pMat));

        mat_data.ambient = mat.ambient;
        mat_data.diffuse = mat.diffuse;
        mat_data.specular = mat.specular;
        mat_data.shininess = mat.shininess;
        mat_data.optical_density = mat.optical_density;
        mat_data.transparency = mat.transparency;
        mat_data.illumination_model = mat.illumination_model;
    
        mat_data.pDiffuse_map = mat.wpDiffuse_map.lock().get();
        mat_data.pSpecular_map = mat.wpSpecular_map.lock().get();
        mat_data.pBump_map = mat.wpBump_map.lock().get();
        mat_data.pAlpha_map = mat.wpAlpha_map.lock().get();
    }
    else if (pMat->get_type() == PBR_MAT)
    {
        const PBRMaterial& mat = *(static_cast<const PBRMaterial*>(pMat));

        mat_data.albedo = mat.albedo;
        mat_data.roughness = mat.roughness;
        mat_data.metallic = mat.metallic;

        mat_data.pAlbedo_map = mat.wpAlbedo_map.lock().get();
        mat_data.pRoughness_map = mat.wpRoughness_map.lock().get();
        mat_data.pMetallic_map = mat.wpMetallic_map.lock().get();
        mat_data.pNormal_map = mat.wpNormal_map.lock().get();
        mat_data.pAO_map = mat.wpAO_map.lock().get();

    } else 
    {
        std::cout << "Invalid Mat Type" << std::endl;
    }
    
}