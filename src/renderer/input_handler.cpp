#include "renderer/input_handler.h"
#include "math/vector.hpp"

bool InputHandler::check_SDL()
{
    return SDL_WasInit(SDL_INIT_VIDEO);
}

bool InputHandler::poll_frame_input(InputState& input, float& mouse_dx, float& mouse_dy)
{
    mouse_dx = 0.0f;
    mouse_dy = 0.0f;
    input = InputState{};  // reset

    while (SDL_PollEvent(&event_))
    {
        switch (event_.type)
        {
            case SDL_EVENT_QUIT:
                return false;

            case SDL_EVENT_KEY_DOWN:
                if (event_.key.key == SDLK_ESCAPE)
                {
                    // Toggle mouse grab on ESC
                    set_mouse_grabbed(!mouse_grabbed_);
                }
                break;

            case SDL_EVENT_MOUSE_MOTION:
                if (mouse_grabbed_)
                {
                    mouse_dx += event_.motion.xrel;
                    mouse_dy += event_.motion.yrel;
                }
                break;

            default:
                break;
        }
    }

    // Poll keyboard state for continuous movement
    const bool* keys = SDL_GetKeyboardState(nullptr);
    input.forward  = keys[SDL_SCANCODE_W];
    input.backward = keys[SDL_SCANCODE_S];
    input.left     = keys[SDL_SCANCODE_A];
    input.right    = keys[SDL_SCANCODE_D];
    input.up       = keys[SDL_SCANCODE_E];
    input.down     = keys[SDL_SCANCODE_Q];
    input.mouse_dx = mouse_dx;
    input.mouse_dy = mouse_dy;

    return true;
}

void InputHandler::set_mouse_grabbed(bool grabbed, SDL_Window* window)
{
    mouse_grabbed_ = grabbed;
    if (window)
        SDL_SetWindowRelativeMouseMode(window, grabbed);
    else if (window_)
        SDL_SetWindowRelativeMouseMode(window_, grabbed);
}

bool InputHandler::handle_event()
{
    switch (event_.type)
    {
        case SDL_EVENT_QUIT:
            return false;

        case SDL_EVENT_KEY_DOWN:
            if (event_.key.key == SDLK_ESCAPE)
            {
                return false;
            }
            break;

        case SDL_EVENT_MOUSE_MOTION:
            break;
    }
    return true;
}
