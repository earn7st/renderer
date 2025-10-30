import trimesh
import os
import json

def read_model_write_json(file_path, output_scene_path):

    MODEL_NAME = os.path.splitext(os.path.basename(file_path))[0]

    mesh = trimesh.load(file_path)

    vertices = mesh.vertices
    normals = mesh.vertex_normals if mesh is not None else None
    texcoords = mesh.visual.uv if hasattr(mesh.visual, 'uv') and mesh.visual.uv is not None else None
    faces = mesh.faces

    model_data = {
        "name" : MODEL_NAME,
        "positions" : vertices.tolist(),
        "normals" : normals.tolist(),
        "faces" : faces.tolist()
    }

    output_file = os.path.join(output_scene_path, f"{MODEL_NAME}.json")
    
    with open(output_file, "w") as f:
        json.dump(model_data, f, indent=2)

def build_scene(output_file):

    camera_data = {

    }

    with open(output_file, "w") as f:
        json.dump(camera_data, f, indent=2)