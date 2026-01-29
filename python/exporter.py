import pywavefront
import json
import os
import numpy as np

def export_to_json(model_name, obj_filepath, scene_context_path):
    if not os.path.exists(scene_context_path):
        os.makedirs(scene_context_path)

    scene = pywavefront.Wavefront(obj_filepath, collect_faces=True, cache=False)
    
    # 1. Load Materials
    mat_library = {}

    for mat_name, mat in scene.materials.items():

        def get_tex_name(texture_obj):
            if texture_obj and hasattr(texture_obj, 'file_name'):
                return os.path.basename(texture_obj.file_name)
            return None

        mat_info = {
            "ambient":   getattr(mat, 'ambient', [0.2, 0.2, 0.2]),   # Ka
            "diffuse":   getattr(mat, 'diffuse', [0.8, 0.8, 0.8]),   # Kd
            "specular":  getattr(mat, 'specular', [1.0, 1.0, 1.0]),  # Ks
            
            "shininess":        getattr(mat, 'shininess', 32.0),     # Ns: 高光指数
            "optical_density":  getattr(mat, 'optical_density', 1.0),# Ni: 折射率 (默认1.0空气)
            "transparency":     getattr(mat, 'transparency', 1.0),   # d/Tr: 1.0是不透明
            "illumination_model": getattr(mat, 'illumination', 2),   # illum: 默认Blinn-Phong
            
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
    global_vertices = [] 

    submeshes_info = []
    vertex_to_index = {}
    submesh_cnt = 0

    for mat_name, mat in scene.materials.items():
        #mesh_name = getattr(mesh, 'name', 'default_mesh')
        mat_name = getattr(mat, 'name', 'default_material')

        print("yeah")

        v_data = mat.vertices
        v_format = mat.vertex_format
        has_uv = 'T2F' in v_format
        has_norm = 'N3F' in v_format
        stride = (2 if has_uv else 0) + (3 if has_norm else 0) + 3
        v_cnt = len(v_data) // stride
        
        for i in range(v_cnt):
            start = i * stride
            curr_offset = 0
            
            u, v = (0.0, 0.0)
            if has_uv:
                u, v = mat.vertices[start : start+2]
                curr_offset += 2
            
            nx, ny, nz = (0.0, 0.0, 0.0)
            if has_norm:
                nx, ny, nz = mat.vertices[start+curr_offset : start+curr_offset+3]
                curr_offset += 3
            
            x, y, z = mat.vertices[start+curr_offset : start+curr_offset+3]
            
            pos_t = list(np.round([x, y, z], 6))
            norm_t = list(np.round([nx, ny, nz], 6))
            uv_t = list(np.round([u, v], 6))
            global_vertices.append({
                "p": pos_t, 
                "n": norm_t,
                "t": uv_t                
            })


    model_info = {
        "name": model_name,
        "vertices": global_vertices,
        "submeshes": submeshes_info
    }

    output_file = os.path.join(scene_context_path, f"{model_name}.json")
    with open(output_file, "w") as f:
        json.dump(model_info, f, indent=2)

    print(f"Exported {model_name}: {len(global_vertices)} vertices, {submesh_cnt} submeshes.")
    