from utils import read_model_write_json
import os
import json

WORKSPACE_PATH = "../"

SCENE_NAME = MODEL_NAME = "spot"
ASSET_PATH = WORKSPACE_PATH + "assets/" + SCENE_NAME + "/"
SCENE_CONTEXT_PATH = WORKSPACE_PATH + "scenes/" + SCENE_NAME + "/"

MODEL_FILEPATH = ASSET_PATH + "spot_control_mesh.obj"


if __name__ == "__main__":

    # 1. Load Model Mesh to {MODEL_NAME}.json
    read_model_write_json(MODEL_FILEPATH, SCENE_CONTEXT_PATH)
    
    # 2. Build scene.json in SCENE_CONTEXT_PATH
    output_filepath = os.path.join(SCENE_CONTEXT_PATH, "scene.json")

    scene_data = {
        "camera" : {
            "position" : [0.0, 0.0, -5.0],
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
                "direction" : [1, 1, 1],
                "color" : [255, 255, 255],
                "intensity" : 1.0
            }
        ],
        "object" : {
            "name" : MODEL_NAME,
            "filename": MODEL_NAME + ".json",
            "transform": {
                "scale": [25.0, 25.0, 25.0],
                "translation": [0.0, -3.0, 0.0],
                "rotation": [0.0, 90.0, 0.0]
            }
        }
        
    }

    with open(output_filepath, "w") as f:
        json.dump(scene_data, f, indent=2)