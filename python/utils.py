import trimesh
import os
import json

def read_model_write_json(model_filepath, scene_context_path):

    MODEL_NAME = os.path.splitext(os.path.basename(model_filepath))[0]

    mesh = trimesh.load(model_filepath)

    vertices = mesh.vertices
    normals = mesh.vertex_normals if mesh is not None else None
    texcoords = mesh.visual.uv if hasattr(mesh.visual, 'uv') and mesh.visual.uv is not None else None
    faces = mesh.faces

    model_data = {
        "name" : MODEL_NAME,
        "group" : False, 
        "positions" : vertices.tolist(),
        "normals" : normals.tolist(),
        "texcoords" : texcoords.tolist() if texcoords is not None else None,
        "faces" : faces.tolist()
    }

    output_file = os.path.join(scene_context_path, f"{MODEL_NAME}.json")
    
    with open(output_file, "w") as f:
        json.dump(model_data, f, indent=2)