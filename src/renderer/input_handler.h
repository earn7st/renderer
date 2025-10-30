#ifndef __INPUT_HANDLER_H__
#define __INPUT_HANDLER_H__

#include <iostream>
#include <SDL3/SDL.h>

class InputHandler
{
public:
    bool poll_event();
    bool handle_event();
    bool check_SDL();

private:
    SDL_Event event_;

    std::unordered_map<int, bool> key_states_;
    bool mouse_button_states_[3] = {false};
};

#endif