#ifndef __MODEL_H__
#define __MODEL_H__

#include "scene/mesh.h"
#include "scene/transform.h"
#include "scene/material.h"

#ifdef _WIN32
    #include <memory>
#endif

struct SubMesh
{
    uint32_t offset = 0;
    uint32_t size = 0;
    std::weak_ptr<Material> wpMaterial;
    Transform local_transform;
};

class Model
{
    
public:
    Model() = default;

    void set_mesh_weak(const std::shared_ptr<Mesh>& mesh);

    const std::weak_ptr<Mesh> mesh_weak() const;

    const SubMesh& get_sub_mesh(uint32_t index) const;
    const std::vector<SubMesh>& get_sub_meshes() const;

    void set_transform(const Transform& transform);
    const Transform& get_transform() const;

    void add_sub_mesh(SubMesh&);

    void print() const;

private:

    // Pointer to Data (in ResourceManager)
    std::weak_ptr<Mesh> wpMesh_;
    Transform transform_;

    std::vector<SubMesh> sub_meshes_;
};

#endif