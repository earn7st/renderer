#include "renderer/input_handler.h"
#include "math/vector.hpp"

bool InputHandler::check_SDL()
{
    return SDL_WasInit(SDL_INIT_VIDEO);
}

void InputHandler::update()
{
    InputState state;
    
    // Poll Mouse Inputs
    while (SDL_PollEvent(&event_)) {
        if (event.type == SDL_EVENT_QUIT) state.wantQuit = true;
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            state.mouseDeltaX = event.motion.xrel;
            state.mouseDeltaY = event.motion.yrel;
        }
    }

    // Get Keyboard States
    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_W]) state.moveDirection.z += 1.0f;
    if (keys[SDL_SCANCODE_S]) state.moveDirection.z -= 1.0f;
    if (keys[SDL_SCANCODE_A]) state.moveDirection.x -= 1.0f;
    if (keys[SDL_SCANCODE_D]) state.moveDirection.x += 1.0f;

    state.moveDirection = normalize(state.moveDirection);

    return state;


}