#include "renderer/input_handler.h"

bool InputHandler::poll_event()
{
    return SDL_PollEvent(&event_);
}

bool InputHandler::handle_event()
{
    switch(event_.type)
    {
        case SDL_EVENT_QUIT:
            return false;
    }

    return true;
}

bool InputHandler::check_SDL()
{
    return SDL_WasInit(SDL_INIT_VIDEO);
}