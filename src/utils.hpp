/* Print functions for all defined structs*/
/* Mesh, Uniform, Varying...*/
#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "renderer/render_types.h"
#include "scene/mesh.h"


void print(const Vertex& vertex)
{
    std::cout << "---- vertex info ----" << std::endl;
    std::cout << "vertex pos: " << vertex.pos << std::endl;
    std::cout << "vertex normal: " << vertex.normal << std::endl;
    std::cout << "vertex texcoord: " << vertex.texcoord << std::endl;
}

void print(const Mesh& mesh)
{
    std::cout << "---- mesh info ----" << std::endl;
    std::cout << "vertices:" << std::endl;
    for (int i = 0; i < mesh.vertices.size(); i++)
    {
        print(mesh.vertices[i]);
    }
}

void print(const Uniform& uniform)
{
    std::cout << "---- uniform info ----" << std::endl;
    std::cout << "view_matrix: " << uniform.view_matrix << std::endl; 
    std::cout << "projection_matrix: " << uniform.projection_matrix << std::endl;
    std::cout << "VP_matrix: " << uniform.VP_matrix << std::endl;
    std::cout << "world_camera_position: " << uniform.world_camera_position << std::endl;
    std::cout << "model_matrix: " << uniform.model_matrix << std::endl;
    std::cout << "MVP_matrix: " << uniform.MVP_matrix << std::endl;
}



#endif