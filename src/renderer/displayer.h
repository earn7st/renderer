#ifndef __DISPLAYER_H__
#define __DISPLAYER_H__

#ifdef __APPLE__
    #define SDL_MAIN_HANDLED
#endif
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "renderer/framebuffer.h"

#define DEFAULT_DISPLAYER_WIDTH 800
#define DEFAULT_DISPLAYER_HEIGHT 600

class Displayer
{
public:
    Displayer();
    Displayer(uint32_t w, uint32_t h);
    ~Displayer();

    int init_SDL();    
    bool present(const Framebuffer&);

    bool check_framebuffer_size(const Framebuffer&);

private:
    uint32_t width_ = DEFAULT_DISPLAYER_WIDTH;
    uint32_t height_ = DEFAULT_DISPLAYER_HEIGHT;
    SDL_Window* window_ = NULL;
    SDL_Renderer* renderer_ = NULL;
    SDL_Texture* texture_ = NULL;
};

#endif