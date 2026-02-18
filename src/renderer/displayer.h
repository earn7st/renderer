#ifndef __DISPLAYER_H__
#define __DISPLAYER_H__

#define SDL_MAIN_HANDLED

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "renderer/framebuffer.h"

#define DEFAULT_DISPLAYER_WIDTH 1600
#define DEFAULT_DISPLAYER_HEIGHT 900

class Displayer
{
public:
    Displayer();
    Displayer(uint32_t w, uint32_t h);
    ~Displayer();

    int init_SDL();
    bool present(const Framebuffer& fb);

private:
    uint32_t width_ = DEFAULT_DISPLAYER_WIDTH;
    uint32_t height_ = DEFAULT_DISPLAYER_HEIGHT;
    SDL_Window* window_ = NULL;
    SDL_Renderer* renderer_ = NULL;
    SDL_Texture* texture_ = NULL;

    std::vector<uint32_t> display_buffer_;
};

#endif