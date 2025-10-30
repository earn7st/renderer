#include "scene/model.h"

void Model::set_pMesh(const Mesh& mesh)
{
    pMesh_ = &mesh;
    return ;
}
void Model::set_pMesh(const Mesh* mesh)
{
    pMesh_ = mesh;
    return ;
}


const Mesh* Model::get_pMesh() const 
{
    return pMesh_;
}

const Mesh& Model::get_mesh() const 
{
    return *pMesh_;
}

const Material* Model::get_pMaterial(uint32_t i) const
{
    assert(i < pMaterials_.size() && "Model::get_pMaterial() : Model Material Index Out of Bound!!");
    return pMaterials_[i];
}

const Material& Model::get_material(uint32_t i) const
{
    assert(i < pMaterials_.size() && "Model::get_material() : Model Material Index Out of Bound!!");
    return *pMaterials_[i];
}

const SubMesh& Model::get_sub_mesh(uint32_t index) const 
{
    assert(index < sub_meshes_.size() &&
            "Model::get_sub_mesh() : Index out of sub_meshes_ bounds");
    return sub_meshes_[index];
}

const std::vector<SubMesh>& Model::get_sub_meshes() const
{
    return sub_meshes_;
}

void Model::add_sub_mesh(SubMesh& submesh)
{
    sub_meshes_.push_back(submesh);
}

void Model::print() const 
{
    const Mesh& mesh  = *pMesh_;
    std::cout << mesh.vertices.size() << " " << mesh.indices.size() << std::endl;
    for(int i = 0; i < sub_meshes_.size(); ++i)
    {
        SubMesh sub_mesh = sub_meshes_[i];
        std::cout << "SubMesh " << i << ":" << std::endl;
        std::cout << "Offset: " << sub_mesh.offset << " Size:" << sub_mesh.size << std::endl;
    }
}