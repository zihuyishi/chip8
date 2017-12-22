//
// Created by saye on 2017/12/21.
//

#ifndef CHIP8_SDLSCREEN_H
#define CHIP8_SDLSCREEN_H

#include "iscreen.h"
#include <SDL.h>

class SDLScreen: public IScreen {
private:
    int m_width;
    int m_height;
    SDL_Window *m_win;
    SDL_Renderer *m_renderer;
    int dpi;
public:
    SDLScreen(int w, int h):
            m_width(w), m_height(h),
            m_win(nullptr), m_renderer(nullptr),
            dpi(4)
    {}

    ~SDLScreen() override;

    int create();
public:
    // override
    void draw(const unsigned char*) override ;
    void clear() override ;
};


#endif //CHIP8_SDLSCREEN_H
