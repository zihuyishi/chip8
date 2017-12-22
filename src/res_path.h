//
// Created by saye on 2017/12/21.
//

#ifndef CHIP8_RES_PATH_H
#define CHIP8_RES_PATH_H

#include <SDL.h>
#include <string>
#include <iostream>

/*
 * Get the resource path for resources located in res/subDir
 * It's assumed the project directory is structured like:
 * bin/
 *  the executable
 * res/
 *  Lesson1/
 *  Lesson2/
 *
 * Paths returned will be Lessons/res/subDir
 */
std::string getResourcePath(const std::string &subDir = "");
#endif //CHIP8_RES_PATH_H
