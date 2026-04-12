#include "renderer/input_handler.h"
#include "math/vector.hpp"

bool InputHandler::check_SDL()
{
    return SDL_WasInit(SDL_INIT_VIDEO);
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