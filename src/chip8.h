//
// Created by saye on 2017/12/21.
//

#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H
#include <string>
#include "iscreen.h"

#define SCR_WIDTH 64
#define SCR_HEIGHT 32

class chip8 {
private:
    unsigned short opcode;
// total 4k memory
    unsigned char memory[4096];
// CPU registers
    unsigned char V[16];

    unsigned short I;
// program counter
    unsigned short pc;

/**
 * The systems memory map:
 *  0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
 *  0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
 *  0x200-0xFFF - Program ROM and work RAM
 */

/**
 * The graphics of the Chip 8 are black and white and the screen has a total of 2048 pixels (64 x 32).
 * This can easily be implemented using an array that hold the pixel state (1 or 0):
 */
    unsigned char gfx[SCR_WIDTH * SCR_HEIGHT];

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short stack[16];
    unsigned short sp;

    bool key[16];
    size_t pro_len;
    bool drawFlag;
    bool waitKey;
    int waitX;
    bool quit;

public:
    void initialize();
    void emulateCycle();

    int loadProgram(const std::string& file);
    void draw(IScreen *screen);
    void keydown(int k);
    void keyup(int k) { key[k] = false; }
    bool needDraw() const { return drawFlag; }
    void terminate() { quit = true; }
    bool isTerminate() const { return quit; }

    void handleOperator();
};


#endif //CHIP8_CHIP8_H
