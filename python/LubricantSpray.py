from model_reader import read_model
import os
import json

WORKSPACE_PATH = "../"

SCENE_NAME = MODEL_NAME = "LubricantSpray"
ASSET_PATH = WORKSPACE_PATH + "assets/" + "LubricantSpray" + "/"
SCENE_CONTEXT_PATH = WORKSPACE_PATH + "scenes/" + SCENE_NAME + "/"

OBJ_FILENAME = "LubricantSpray.obj"
OBJ_FILEPATH = os.path.join(ASSET_PATH, OBJ_FILENAME)

if __name__ == "__main__":

    # 1. Read Model: Material JSON Data -> mat_library, Model Mesh JSON Data -> model_info
    mat_library, model_info = read_model(MODEL_NAME, OBJ_FILEPATH, SCENE_CONTEXT_PATH, "PBR")

    # case-to-case modification
    mat_name = "lubricant_spray"
    mat_library[mat_name]["textures"]["roughness_map"] = "lubricant_spray_rough_2k.jpg"
    mat_library[mat_name]["textures"]["metallic_map"] = "lubricant_spray_metal_2k.jpg"
    mat_library[mat_name]["textures"]["ao_map"] = "lubricant_spray_ao_2k.jpg"

    # Produce [MODEL_NAME]_material.json file
    output_file = os.path.join(SCENE_CONTEXT_PATH, f"{MODEL_NAME}_materials.json")
    with open(output_file, "w") as f:
        json.dump(mat_library, f, indent=2)

    # Produce [MODEL_NAME].json file
    output_file = os.path.join(SCENE_CONTEXT_PATH, f"{MODEL_NAME}.json")
    with open(output_file, "w") as f:
        json.dump(model_info, f, indent=2)
    
    # 2. Build scene.json in SCENE_CONTEXT_PATH
    output_filepath = os.path.join(SCENE_CONTEXT_PATH, "scene.json")

    scene_info = {
        "asset_path" : ASSET_PATH,
        "camera" : {
            "position" : [0.0, 0.0, 4.0],
            "center" : [0.0, 0.0, 0.0],
            "world_up" : [0.0, 1.0, 0.0],
            "aspect" : float(16/9),
            "fov" : 60.0,
            "n_plane" : 0.01,
            "f_plane" : 5.0
        },
        "lights": [
            {
                "type" : "Directional",
                "direction" : [-1, -1, -1],
                "color" : [1, 1, 1],
                "intensity" : 1.0
            },
            {
                "type" : "Directional",
                "direction" : [1, -1, -1],
                "color" : [1, 1, 1],
                "intensity" : 1.0
            }
        ],
        "object" : {
            "name" : MODEL_NAME,
            "filename": MODEL_NAME + ".json",
            "transform": {
                "scale": [17.0, 17.0, 17.0],
                "translation": [0.0, -1.5, 0.0],
                "rotation": [15.0, 0.0, 0.0] 
            }
        }
        
    }

    with open(output_filepath, "w") as f:
        json.dump(scene_info, f, indent=2)