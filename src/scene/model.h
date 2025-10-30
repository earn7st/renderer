#ifndef __MODEL_H__
#define __MODEL_H__

#include "scene/mesh.h"
#include "scene/transform.h"
#include "scene/material.h"

struct SubMesh
{
    uint32_t offset = 0;
    uint32_t size = 0;
    uint32_t material_index = 0;
    Transform local_transform;
};

class Model
{
    
public:
    Model() = default;

    void set_pMesh(const Mesh& mesh);
    void set_pMesh(const Mesh* mesh);

    const Mesh* get_pMesh() const;
    const Mesh& get_mesh() const;
    const Material* get_pMaterial(uint32_t index) const;
    const Material& get_material(uint32_t index) const;

    const SubMesh& get_sub_mesh(uint32_t index) const;
    const std::vector<SubMesh>& get_sub_meshes() const;

    void add_sub_mesh(SubMesh&);

    void print() const;

private:

    // Pointer to Data (in ResourceManager)
    const Mesh* pMesh_;
    Transform transform_;
    std::vector<Material*> pMaterials_;

    std::vector<SubMesh> sub_meshes_;
};

#endif