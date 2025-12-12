#include "scene/model.h"

void Model::set_mesh_weak(const std::shared_ptr<Mesh>& mesh)
{
    wpMesh_ = mesh;
}

const std::weak_ptr<Mesh> Model::mesh_weak() const 
{
    return wpMesh_;
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

void Model::set_transform(const Transform& transform)
{
    transform_ = transform;
}

const Transform& Model::get_transform() const
{
    return transform_;
}

void Model::add_sub_mesh(SubMesh& submesh)
{
    sub_meshes_.push_back(submesh);
}

void Model::print() const 
{
    std::shared_ptr<Mesh> spMesh = wpMesh_.lock();
    const Mesh& mesh = *spMesh;
    std::cout << mesh.vertices.size() << " " << mesh.indices.size() << std::endl;
    for(int i = 0; i < sub_meshes_.size(); ++i)
    {
        SubMesh sub_mesh = sub_meshes_[i];
        std::cout << "SubMesh " << i << ":" << std::endl;
        std::cout << "Offset: " << sub_mesh.offset << " Size:" << sub_mesh.size << std::endl;
    }

}