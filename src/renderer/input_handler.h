#ifndef __INPUT_HANDLER_H__
#define __INPUT_HANDLER_H__

#include <iostream>
#include <SDL3/SDL.h>
#include <unordered_map>

struct InputState
{
    // Keyboard
    Vec3f moveDirection(0.f);

    // Mouse
    float mouseDeltaX = 0.f;
    float mouseDeltaY = 0.f;
};

class InputHandler
{
public:
    
    const InputState& get_input_state() const { return input_state_; }

    bool check_SDL();
    void update();
    

private:
    SDL_Event event_;

    std::unordered_map<int, bool> key_states_;
    bool mouse_button_states_[3] = {false};
};

#endif