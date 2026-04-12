#include "scene/model.h"

void Model::set_mesh_weak(const std::shared_ptr<Mesh>& mesh)
{
    wpMesh_ = mesh;
}

const std::weak_ptr<Mesh> Model::mesh_weak() const 
{
    return wpMesh_;
}

const SubMesh& Model::get_submesh(uint32_t index) const 
{
    assert(index < submeshes_.size() &&
            "Model::get_submesh() : Index out of submeshes_ bounds");
    return submeshes_[index];
}

const std::vector<SubMesh>& Model::get_submeshes() const
{
    return submeshes_;
}

void Model::set_transform(const Transform& transform)
{
    transform_ = transform;
}

const Transform& Model::get_transform() const
{
    return transform_;
}

void Model::add_submesh(SubMesh& submesh)
{
    submeshes_.push_back(submesh);
}

void Model::print_info() const 
{
    std::cout << "---- Model ----" << std::endl;
    
    std::shared_ptr<Mesh> spMesh = wpMesh_.lock();
    const Mesh& mesh = *spMesh;
    std::cout << "Num Vertices: " << mesh.vertices.size() << " Num Indices: " << mesh.indices.size() << std::endl;
    for(size_t i = 0; i < submeshes_.size(); ++i)
    {
        SubMesh submesh = submeshes_[i];
        std::cout << "SubMesh " << i << ":" << std::endl;
        std::cout << "Offset: " << submesh.index_offset << " Size:" << submesh.index_count << std::endl;
    }
    std::cout << "Transform: " << std::endl;
    std::cout << "scale: " << transform_.scale << std::endl;
    std::cout << "translation: " << transform_.translation << std::endl;
    std::cout << "rotation: " << transform_.rotation << std::endl;
}