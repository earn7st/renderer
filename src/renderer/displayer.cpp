#include "renderer/displayer.h"

Displayer::Displayer()
{
    init_SDL();
}

Displayer::Displayer(uint32_t w, uint32_t h) : width_(w), height_(h)
{
    init_SDL();
}

Displayer::~Displayer()
{
    if (texture_) {
        SDL_DestroyTexture(texture_);
    }
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

int Displayer::init_SDL()
{
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL Init Failed: %s", SDL_GetError());
        return -1;
    }

    window_ = SDL_CreateWindow("Displayer", width_, height_, SDL_WINDOW_RESIZABLE);
    if (!window_)
    {
        SDL_Log("SDL Create Window Failed: %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (!renderer_) {
        SDL_Log("SDL Create Renderer Failed: %s\n", SDL_GetError());
        return -1;
    }
    
    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width_, height_);
    if (!texture_)
    {
        SDL_Log("SDL Create Texture Failed: %s\n", SDL_GetError());
        return -1;
    }
    
    return 0;
}

bool Displayer::present(const Framebuffer& fb)
{
    const std::vector<RGBA> float_pixels = fb.get_color_data();

    display_buffer_.resize(float_pixels.size());    

    if (!texture_ || !renderer_) {
        SDL_Log("Displayer::present(): SDL::Invalid pixel data or SDL objects.");
        return false;
    }

    for (int i = 0; i < float_pixels.size(); ++i)
    {
        RGBA c = float_pixels[i];
        uint8_t r = static_cast<uint8_t>(std::clamp(c.x_, 0.f, 1.f) * 255.f);
        uint8_t g = static_cast<uint8_t>(std::clamp(c.y_, 0.f, 1.f) * 255.f);
        uint8_t b = static_cast<uint8_t>(std::clamp(c.z_, 0.f, 1.f) * 255.f);
        uint8_t a = static_cast<uint8_t>(std::clamp(c.w_, 0.f, 1.f) * 255.f);

        display_buffer_[i] = (a << 24) | (b << 16) | (g << 8) | (r);
    }

    SDL_UpdateTexture(texture_, nullptr, display_buffer_.data(), width_ * sizeof(uint32_t));
    SDL_RenderTexture(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);

    return true;
}