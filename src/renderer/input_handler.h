#ifndef __INPUT_HANDLER_H__
#define __INPUT_HANDLER_H__

#include <iostream>
#include <SDL3/SDL.h>

class InputHandler
{
public:

    bool check_SDL();
    bool poll_event() { return SDL_PollEvent(&event_); }
    bool handle_event();

private:
    SDL_Event event_;
};

#endif