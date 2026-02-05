import pywavefront
import json
import os
import numpy as np

global_vertices = []
global_indices = []
vertex_map = {}

def get_vertex_index(pos, norm, uv):
    key = (tuple(pos), tuple(norm), tuple(uv))
    if key not in vertex_map:
        vertex_map[key] = len(global_vertices)
        global_vertices.append({
            "p": pos,
            "n": norm,
            "t": uv
        })
    return vertex_map[key]

def export_to_json(model_name, obj_filepath, scene_context_path):
    if not os.path.exists(scene_context_path):
        os.makedirs(scene_context_path)

    print("Exporting Model from" + obj_filepath)

    scene = pywavefront.Wavefront(obj_filepath, collect_faces=True, cache=False)
    
    # 1. Load Materials
    mat_library = {}

    for mat_name, mat in scene.materials.items():

        def get_tex_name(texture_obj):
            if texture_obj and hasattr(texture_obj, 'file_name'):
                name = os.path.basename(texture_obj.file_name)
                name = name.strip()
                name = name.split()[-1]
                return name
            return None

        mat_info = {
            "ambient":   getattr(mat, 'ambient', [0.2, 0.2, 0.2]),   # Ka
            "diffuse":   getattr(mat, 'diffuse', [0.8, 0.8, 0.8]),   # Kd
            "specular":  getattr(mat, 'specular', [1.0, 1.0, 1.0]),  # Ks
            
            "shininess":        getattr(mat, 'shininess', 32.0),     # Ns
            "optical_density":  getattr(mat, 'optical_density', 1.0),# Ni
            "transparency":     getattr(mat, 'transparency', 1.0),   # d/Tr
            "illumination_model": getattr(mat, 'illumination', 2),   # illum
            
            "textures": {
                "diffuse_map":  get_tex_name(getattr(mat, 'texture', None)),
                "specular_map": get_tex_name(getattr(mat, 'texture_specular', None)),
                "bump_map":     get_tex_name(getattr(mat, 'texture_bump', None)),
                "alpha_map":    get_tex_name(getattr(mat, 'texture_opacity', None))
            }
        }

        mat_library[mat_name] = mat_info
    
    output_file = os.path.join(scene_context_path, f"{model_name}_materials.json")
    with open(output_file, "w") as f:
        json.dump(mat_library, f, indent=2)
    
    print(f"Materials for {model_name} (with textures) exported.")

    # 2. Load Mesh
    submeshes_info = []
    submesh_cnt = 0

    for mat_name, mat in scene.materials.items():
        start_index = len(global_indices)

        v_data = mat.vertices
        v_format = mat.vertex_format

        elements = v_format.split('_')
        stride = 0
        for e in elements:
            if e == 'T2F': stride += 2
            elif e == 'N3F': stride += 3
            elif e == 'V3F': stride += 3

        for i in range(0, len(v_data), stride):
            offset = 0

            if 'T2F' in elements:
                u, v = v_data[i+offset:i+offset+2]
                offset += 2
            else:
                u, v = -1.0, -1.0

            if 'N3F' in elements:
                nx, ny, nz = v_data[i+offset:i+offset+3]
                offset += 3
            else:
                nx, ny, nz = 0.0, 0.0, 0.0

            x, y, z = v_data[i+offset:i+offset+3]

            idx = get_vertex_index(
                [x, y, z],
                [nx, ny, nz],
                [u, v]
            )

            global_indices.append(idx)

        index_count = len(global_indices) - start_index

        if index_count > 0:
            submeshes_info.append({
                "material": mat_name,
                "offset": start_index,
                "size": index_count
            }) 
            submesh_cnt += 1

    model_info = {
        "name": model_name,
        "vertices": global_vertices,
        "indices": global_indices,
        "submeshes": submeshes_info
    }

    output_file = os.path.join(scene_context_path, f"{model_name}.json")
    with open(output_file, "w") as f:
        json.dump(model_info, f, indent=2)

    print(f"Exported {model_name}: {len(global_vertices)} vertices, {submesh_cnt} submeshes.")
    