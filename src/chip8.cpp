//
// Created by saye on 2017/12/21.
//

#include "chip8.h"
#include "res_path.h"
#include <random>

constexpr unsigned char chip8_fontset[80] =
        {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };


void chip8::initialize() {
    pc = 0x200; // Promgram counter starts at 0x200
    opcode = 0;
    I = 0;
    sp = 0;
    drawFlag = false;
    quit = false;
    waitKey = false;

    for (bool &i : key) {
        i = false;
    }

    // Load fontset
    for (int i = 0; i < 80; i++) {
        memory[i] = chip8_fontset[i];
    }
}

void chip8::emulateCycle() {
    // Fetch Opcode
    if (!waitKey) {
        opcode = memory[pc] << 8 | memory[pc + 1];
        handleOperator();
        std::cout << "opcode " << std::hex << opcode << std::endl;
    }

    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        if (sound_timer == 1) {
            printf("BEEP!\n");
            --sound_timer;
        }
    }
}

void chip8::handleOperator() {
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0: // 0x00E0 clear screen
                    for (unsigned char &i : gfx) {
                        i = 0;
                    }
                    drawFlag = true;
                    pc += 2;
                    break;
                case 0x00EE: // 0x00EE return from a subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;
        case 0x1000: // 1NNN jumps to address NNN
            pc = static_cast<unsigned short>(opcode & 0x0FFF);
            break;
        case 0x2000: // 2NNN calls subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = static_cast<unsigned short>(opcode & 0x0FFF);
            break;
        case 0x3000: // 3XNN skips next pc if VX == NN
        {
            auto X = (opcode & 0x0F00) >> 8;
            auto NN = opcode & 0x00FF;
            if (V[X] == NN) {
                pc += 2;
            }
            pc += 2;
        }
            break;
        case 0x4000: // 4XNN skips next pc if VX != NN
        {
            auto X = (opcode & 0x0F00) >> 8;
            auto NN = opcode & 0x00FF;
            if (V[X] != NN) {
                pc += 2;
            }
            pc += 2;
        }
            break;
        case 0x5000: // 5XY0 skips next pc if VX == VY
        {
            auto X = (opcode & 0x0F00) >> 8;
            auto Y = (opcode & 0x00F0) >> 4;
            if (V[X] == V[Y]) {
                pc += 2;
            }
            pc += 2;
        }
            break;
        case 0x6000: // 6XNN sets VX to NN
            V[(opcode & 0x0F00) >> 8] = static_cast<unsigned char>(opcode & 0x00FF);
            pc += 2;
            break;
        case 0x7000: // 7XNN adds NN to VX (Carry flag is not changed)
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
        case 0x8000:
            switch (opcode & 0x00F) {
                case 0x0000: // 8XY0 VX = VY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0001: // 8XY1 bit sets VX to VX or VY
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0002: // 8XY2 bit sets VX to VX and VY
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0003: // 8XY3 bit sets VX to VX xor VY
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0004: // 8XY4 math Vx += Vy
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1; //carry
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0005: // 8XY5 math VX -= VY, VF is set to 0 where borrow
                    if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
                        V[0xF] = 0;
                    } else {
                        V[0xF] = 1;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0006: // 8XY6 bit shifts VY right by one, and assign to VX. VF is set to the value of the least significant bit of VY before the shift.
                    V[0xF] = static_cast<unsigned char>(V[(opcode & 0x00F0) >> 4] & 0x1);
                    V[(opcode & 0x0F00) >> 8] =
                            (V[(opcode & 0x00F0) >> 4] = V[(opcode & 0x00F0) >> 4] >> 1);
                    pc += 2;
                    break;
                case 0x0007: // 8XY7 math VX = VY - VX, VF is set to 0 where borrow
                    if (V[(opcode & 0x00F0) >> 4] < V[(opcode & 0x0F00) >> 8]) {
                        V[0xF] = 0;
                    } else {
                        V[0xF] = 1;
                    }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x000E: // 8XYE bit shifts VY right by one, and assign to VX. VF is set to the value of the least significant bit of VY before the shift.
                    V[0xF] = static_cast<unsigned char>((V[(opcode & 0x00F0) >> 4] & 0x80) >> 7);
                    V[(opcode & 0x0F00) >> 8] =
                            (V[(opcode & 0x00F0) >> 4] = V[(opcode & 0x00F0) >> 4] << 1);
                    pc += 2;
                    break;
                default:
                    printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
            }
            break;
        case 0x9000: // 9XY0 if Vx != Vy then jump next pc
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;
        case 0xA000: // ANNN MEM sets I to NNN
            I = static_cast<unsigned short>(opcode & 0x0FFF);
            pc += 2;
            break;
        case 0xB000:
            pc = static_cast<unsigned short>(V[0] + (opcode & 0x0FFF));
            break;
        case 0xC000: // CXNN VX = rand() & NN
        {
            std::__1::random_device rd;
            std::__1::uniform_int_distribution<unsigned char> dist(0, 255);
            V[(opcode & 0x0F00) >> 8] = static_cast<unsigned char>(dist(rd) & (opcode & 0x00FF));
            pc += 2;
        }
            break;
        case 0xD000:
        {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = static_cast<unsigned short>(opcode & 0x000F);
            unsigned short pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                            V[0xF] = 1;
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
        }
            break;
        case 0xE000:
        std::cout << "wait for key" << std::endl;
            switch (opcode & 0x00FF) {
                case 0x009E:
                    if (key[V[(opcode & 0x0F00) >> 8]]) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;
                case 0x00A1:
                    if (!key[V[(opcode & 0x0F00) >> 8]]) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;
                default:
                    std::cerr << "unkown op " << opcode << std::endl;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: // FX07 sets VX to the value of delay time
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;
                case 0x000A: // FX0A A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
                std::cout << "pause for key" << std::endl;
                    waitKey = true;
                    waitX = (opcode & 0x0F00) >> 8;
                    pc += 2;
                    break;
                case 0x0015: // FX15 set delay timer to VX
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x0018: // FX18 set sound timer to VX
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x001E:
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
                    I = static_cast<unsigned short>(V[(opcode & 0x0F00) >> 8] * 0x5);
                    pc += 2;
                    break;
                case 0x0033:
                    memory[I]     = static_cast<unsigned char>(V[(opcode & 0x0F00) >> 8] / 100);
                    memory[I + 1] = static_cast<unsigned char>((V[(opcode & 0x0F00) >> 8] / 10) % 10);
                    memory[I + 2] = static_cast<unsigned char>((V[(opcode & 0x0F00) >> 8] % 100) % 10);
                    pc += 2;
                    break;
                case 0x0055: // FX55: Stores V0 to VX in memory starting at address I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        memory[I + i] = V[i];

                    // On the original interpreter, when the operation is done, I = I + X + 1.
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;

                case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        V[i] = memory[I + i];

                    // On the original interpreter, when the operation is done, I = I + X + 1.
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;
                default:
                    printf("Unknown opcode [0xF000]");
            }
            break;
        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }
}

int chip8::loadProgram(const std::string &file) {
    std::cout << "load " << file << std::endl;
    FILE* fd = fopen(file.c_str(), "r");
    if (fd == nullptr) {
        return -1;
    }
    std::cout << "load into memory" << std::endl;
    size_t read_size = fread(memory + 512, 1, 4096 - 512, fd);
    if (ferror(fd) != 0) {
        fclose(fd);
        return -1;
    }
    std::cout << "read " << read_size << " bytes" << std::endl;

    pro_len = read_size;
    fclose(fd);
    return 0;
}

void chip8::draw(IScreen *screen) {
    drawFlag = false;
    screen->draw(gfx);
}

void chip8::keydown(int k) {
    key[k] = true;
    if (waitKey) {
        waitKey = false;
        V[waitX] = static_cast<unsigned char>(k);
    }
}
