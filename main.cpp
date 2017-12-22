#include <iostream>
#include <SDL.h>
#include "src/res_path.h"
#include "src/chip8.h"
#include "src/sdlscreen.h"
#include <thread>

using namespace std::chrono;
chip8 myChip8;

void handleKeyboard();

int main() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return 1;
    }
    auto screen = new SDLScreen(SCR_WIDTH, SCR_HEIGHT);
    if (screen->create() != 0) {
        std::cerr << "SDL_Create error" << std::endl;
        return 1;
    }
    std::cout << "init chip8" << std::endl;
    myChip8.initialize();
    std::cout << "load program" << std::endl;
    std::string path = "/Users/saye/Documents/mycode/emulator/chip8/res/invaders.c8";
    //std::string path = getResourcePath() + "pong.rom";
    if (myChip8.loadProgram(path) != 0) {
        std::cerr << "load program error" << std::endl;
        return 1;
    }
    std::cout << "program loaded" << std::endl;
    while (true) {
        auto startTime = steady_clock::now();
        handleKeyboard();
        myChip8.emulateCycle();
        if (myChip8.isTerminate()) {
            break;
        }
        if (myChip8.needDraw()) {
            myChip8.draw(screen);
        }
        auto endTime = steady_clock::now();
        auto time_span = duration_cast<duration<double>>(endTime - startTime);
        auto delay = round(1000 / 180 - time_span.count() * 1000);
        if (delay > 0) {
            std::cout << "delay " << delay << " ms" << std::endl;
            SDL_Delay(static_cast<Uint32>(delay));
        }
    }
    return 0;
}

int keymap(int key) {
    switch (key) {
        case SDLK_1:
            return 1;
        case SDLK_2:
            return 2;
        case SDLK_3:
            return 3;
        case SDLK_4:
            return 12;
        case SDLK_q:
            return 4;
        case SDLK_w:
            return 5;
        case SDLK_e:
            return 6;
        case SDLK_r:
            return 13;
        case SDLK_a:
            return 7;
        case SDLK_s:
            return 8;
        case SDLK_d:
            return 9;
        case SDLK_f:
            return 14;
        case SDLK_z:
            return 10;
        case SDLK_x:
            return 0;
        case SDLK_c:
            return 11;
        case SDLK_v:
            return 15;
        default:
            return -1;

    }
}

void handleKeyboard() {
    SDL_Event event{};
    SDL_PollEvent(&event);
    // check for messages
    switch (event.type) {
        // exit if the window is closed
        case SDL_QUIT:
            myChip8.terminate();
            return ;
            // check for keypresses
        case SDL_KEYDOWN:
        {
            auto key = keymap(event.key.keysym.sym);
            if (key != -1) {
                std::cout << "key " << key << " down" << std::endl;
                myChip8.keydown(key);
            }
        }
            break;
        case SDL_KEYUP:
        {
            auto key = keymap(event.key.keysym.sym);
            if (key != -1) {
                std::cout << "key " << key << " up" << std::endl;
                myChip8.keyup(key);
            }
        }
            break;
        default:
            break;
    }
}