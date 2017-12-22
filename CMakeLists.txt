cmake_minimum_required(VERSION 3.9)
project(chip8)

include_directories("thirdpart/SDL/include" include)
set(CMAKE_CXX_STANDARD 11)

add_executable(chip8 main.cpp src/chip8.cpp src/chip8.h src/iscreen.h src/sdlscreen.cpp src/sdlscreen.h src/res_path.cpp)
target_link_libraries(chip8 SDL2)