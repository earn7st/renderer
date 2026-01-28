#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <functional>

#include "renderer/framebuffer.h"
#include "renderer/render_types.h"
#include "renderer/shader.h"
#include "renderer/rasterizer.h"

#include "scene/mesh.h"
#include "scene/camera.h"
#include "scene/scene.h"

class Renderer
{
public:
    Renderer() = default;
    Renderer(RenderState rs);

    bool attach_framebuffer(Framebuffer*);
    void set_render_state(const RenderState&);

    void render(const Scene&);
    void draw_model(const Model&);   // For Test
    void draw_sub_mesh(const SubMesh&); // For Test

    void update_per_frame_uniform(const Scene&);   // call when frame start
    void update_per_model_uniform(const Transform&);    // call when drawing model
    void update_per_sub_mesh_uniform(const SubMesh&);

    // pipeline
    void draw_call(const Mesh& mesh, uint32_t offset, uint32_t size, const Shader* shader);

private:
    RenderState render_state_;
    Framebuffer* fb_;
    Uniform uniform_;
    LightUniform light_uniform_;
    Rasterizer rasterizer_;
};

#endif