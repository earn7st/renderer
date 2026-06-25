#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <functional>
#include <memory>

#include "renderer/framebuffer.h"
#include "renderer/render_types.hpp"
#include "renderer/shader.h"
#include "renderer/rasterizer.h"
#include "renderer/shader_constants.h"
#include "renderer/tile_binner.h"
#include "renderer/job_system.h"

#include "scene/mesh.h"
#include "scene/camera.h"
#include "scene/fps_camera.h"
#include "scene/scene.h"

// Lightweight draw command — one per submesh, not per triangle.
struct DrawBatch
{
    const Mesh*    mesh;
    uint32_t       first_index;     // offset into mesh.indices
    uint32_t       triangle_count;  // number of triangles (index_count / 3)
    const Shader*  shader;
    ShaderConstants constants;      // one copy per submesh (amortised)
};

class Renderer
{
public:
    Renderer() = default;
    Renderer(RenderState rs);

    bool attach_framebuffer(Framebuffer*);
    void set_render_state(const RenderState&);
    void initialize_lights(const Scene&);

    // Single-threaded render
    void render(const Scene&, const FpsCamera& camera);

    // Tile-parallel render (bins triangles, dispatches tiles to worker threads)
    void render_parallel(const Scene&, const FpsCamera& camera);

    void draw_model(const Model&, ShaderConstants&);
    void draw_submesh(const SubMesh&, ShaderConstants&);

    // Collect draw batches from the scene (fast, serial)
    void collect_draw_batches(const Scene&, std::vector<DrawBatch>& batches);

    void update_per_frame_uniform(const FpsCamera& camera);   // call when frame start
    void update_per_model_uniform(const Transform&);    // call when drawing model
    void update_per_submesh_uniform(const SubMesh&);

    // Access job system for configuration
    JobSystem& job_system() { return job_system_; }

private:
    RenderState render_state_;
    Framebuffer* fb_;
    Uniform uniform_;
    LightUniform light_uniform_;
    Rasterizer rasterizer_;
    JobSystem  job_system_;

    void prepare_mat_data(ShaderConstants& shader_constants, const Material* pMat) const;

    void draw_call(const Mesh& mesh, uint32_t offset, uint32_t size,
                   const Shader* shader, const ShaderConstants& shader_constants);

    // Internal: rasterize a single tile in parallel mode
    void rasterize_tile_job(const TileBin& tile, const TileBinner& binner,
                            const std::vector<DrawBatch>& batches);
};

#endif