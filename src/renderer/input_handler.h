#ifndef __INPUT_HANDLER_H__
#define __INPUT_HANDLER_H__

#include <iostream>
#include <SDL3/SDL.h>

#include "scene/fps_camera.h"   // for InputState

class InputHandler
{
public:
    bool check_SDL();

    // Poll all pending events and accumulate into an InputState.
    // Returns false if the user requested quit (ESC / window close).
    bool poll_frame_input(InputState& input, float& mouse_dx, float& mouse_dy);

    // Legacy single-event interface (kept for backward compat)
    bool poll_event() { return SDL_PollEvent(&event_); }
    bool handle_event();

    // Toggle mouse grab for FPS look (needs valid SDL window)
    void set_mouse_grabbed(bool grabbed, SDL_Window* window = nullptr);

    // Store window reference for mouse grab
    void set_window(SDL_Window* w) { window_ = w; }

private:
    SDL_Event  event_;
    bool       mouse_grabbed_ = false;
    SDL_Window* window_ = nullptr;
};

#endif
