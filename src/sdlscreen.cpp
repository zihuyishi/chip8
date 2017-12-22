//
// Created by saye on 2017/12/21.
//

#include <iostream>
#include "sdlscreen.h"


void SDLScreen::draw(const unsigned char *buf) {
    int count = 0;
    auto points = new SDL_Point[m_width * m_height * dpi * dpi];

    for (int i = 0; i < m_width; i++) {
        for (int j = 0; j < m_height; j++) {
            if (buf[j * m_width + i] == 1) {
                for (int px = 0; px < dpi; px++) {
                    for (int py = 0; py < dpi; py++) {
                        points[count] = SDL_Point {x: i * dpi + px, y: j * dpi + py};
                        ++count;
                    }
                }
            }
        }
    }

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoints(m_renderer, points, count);
    SDL_RenderPresent(m_renderer);
    delete[] points;
}

void SDLScreen::clear() {
    SDL_RenderClear(m_renderer);
}

int SDLScreen::create() {
    SDL_CreateWindowAndRenderer(m_width * dpi, m_height * dpi, SDL_WINDOW_SHOWN, &m_win, &m_renderer);
    if (m_win == nullptr || m_renderer == nullptr) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        return 1;
    }
    return 0;
}

SDLScreen::~SDLScreen() {
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if (m_win) {
        SDL_DestroyWindow(m_win);
        m_win = nullptr;
    }
}
