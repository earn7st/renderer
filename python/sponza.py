from exporter import export_to_json
import os
import json

WORKSPACE_PATH = "../"

SCENE_NAME = MODEL_NAME = "sponza"
ASSET_PATH = WORKSPACE_PATH + "assets/" + "sponza" + "/"
SCENE_CONTEXT_PATH   = WORKSPACE_PATH + "scenes/" + SCENE_NAME + "/"

OBJ_FILEPATH = os.path.join(ASSET_PATH, MODEL_NAME + ".obj")

if __name__ == "__main__":

    # 1. Load Model Mesh (-> {MODEL_NAME}.json) and Materials (-> materials.json)
    export_to_json(MODEL_NAME, OBJ_FILEPATH, SCENE_CONTEXT_PATH)
    
    # 2. Build scene.json in SCENE_CONTEXT_PATH
    output_filepath = os.path.join(SCENE_CONTEXT_PATH, "scene.json")

    scene_info = {
        "asset_path": ASSET_PATH,
        "camera" : {
            "position" : [10.0, 10.0, 10.0],
            "center" : [0.0, 0.0, 0.0],
            "world_up" : [0.0, 1.0, 0.0],
            "aspect" : float(4/3),
            "fov" : 60.0,
            "n_plane" : 0.01,
            "f_plane" : 10.0
        },
        "lights": [
            {
                "type" : "Directional",
                "direction" : [-1, -1, -1],
                "color" : [1, 1, 1],
                "intensity" : 1.0
            }
        ],
        "object" : {
            "name" : MODEL_NAME,
            "filename": MODEL_NAME + ".json",
            "transform": {
                "scale": [1.0, 1.0, 1.0],
                "translation": [0.0, 0.0, 0.0],
                "rotation": [0.0, 0.0, 0.0]
            }
        }
        
    }

    with open(output_filepath, "w") as f:
        json.dump(scene_info, f, indent=2)