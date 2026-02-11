#ifndef __MESH_H__
#define __MESH_H__

#ifdef _WIN32
    #include <vector>
#endif

#include "renderer/render_types.hpp"

struct Mesh
{
    uint32_t num_vertices = 0;
    std::vector<Vertex> vertices;

    uint32_t num_faces = 0;
    std::vector<uint32_t> indices;
};

#endif