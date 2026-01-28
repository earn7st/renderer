import trimesh
import os
import json
import numpy as np

# Redundant vertices/normals/texcoords when reading multi-submeshes model 
def export_to_json(filepath, scene_context_path):
    
    MODEL_NAME = os.path.splitext(os.path.basename(os.path.normpath(scene_context_path)))[0]
    scene = trimesh.load(filepath)
    model_data = {}

    if isinstance(scene, trimesh.Trimesh):
        scene = trimesh.Scene(scene)    
    
    global_positions = []
    global_normals = []
    global_texcoords = []

    submeshes_data = []

    cnt = 0
    vertex_to_index = {}
    for name, submesh in scene.geometry.items():

        submesh_global_indices = []

        mat_name = "default"
        if hasattr(submesh.visual, 'material'):
            mat_name = submesh.visual.material.name

        print(f"Submesh Name: {name}")
        print(f"Vertices: {len(submesh.vertices)}")
        print(f"Faces: {len(submesh.faces)}")

        vertices = submesh.vertices
        normals = submesh.vertex_normals if submesh is not None else None
        texcoords = submesh.visual.uv if hasattr(submesh.visual, 'uv') and submesh.visual.uv is not None else None

        local_to_global_index = {}

        for i in range(len(vertices)):
            pos = tuple(np.round(vertices[i], 6))
            norm = tuple(np.round(normals[i], 6))
            uv = tuple(np.round(texcoords[i], 6)) if texcoords is not None else None
            v_key = (pos, norm, uv)

            if v_key not in vertex_to_index:
                idx = len(global_positions)
                vertex_to_index[v_key] = idx
                global_positions.append(list(pos))
                global_normals.append(list(norm))
                global_texcoords.append(list(uv)) if uv is not None else None

            local_to_global_index[i] = vertex_to_index[v_key]

        for face in submesh.faces:
            for local_index in face:
                submesh_global_indices.append(local_to_global_index[local_index])

        submesh_data = {
            "submesh index" : cnt,
            "name" : name,
            "material" : mat_name,
            "indices" : submesh_global_indices
        }

        submeshes_data.append(submesh_data)

        cnt += 1
    
    model_data = {
        "name" : MODEL_NAME,
        "positions" : global_positions,
        "normals" : global_normals,
        "texcoords" : global_texcoords,
        "submeshes" : submeshes_data
    }

    if not os.path.exists(scene_context_path):
        os.makedirs(scene_context_path)
        print("Context path folder \"" + MODEL_NAME + "\" created.")
    output_file = os.path.join(scene_context_path, f"{MODEL_NAME}.json")
    
    with open(output_file, "w") as f:
        json.dump(model_data, f, indent=2)
    