//
// Created by saye on 2017/12/21.
//

#ifndef CHIP8_ISCREEN_H
#define CHIP8_ISCREEN_H


class IScreen {
public:
    virtual ~IScreen() = default;
    virtual void draw(const unsigned char *buf) = 0;
    virtual void clear() = 0;
};


#endif //CHIP8_ISCREEN_H
