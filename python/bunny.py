from utils import read_model_write_json
from utils import build_scene

WORKSPACE_PATH = "../"

SCENE_NAME = "bunny"
ASSET_PATH = WORKSPACE_PATH + "assets/" + SCENE_NAME + "/"
SCENE_PATH = WORKSPACE_PATH + "scenes/" + SCENE_NAME + "/"

MODEL_PATH = ASSET_PATH + "bunny.obj"

if __name__ == "__main__":
    build_scene(SCENE_PATH + "scene.json")
    read_model_write_json(MODEL_PATH, SCENE_PATH)