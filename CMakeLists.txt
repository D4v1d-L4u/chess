cmake_minimum_required(VERSION 3.24)
project(chess C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -pedantic -Werror)


add_executable(chess main.c chessboard.h chessboard.c stringbuilder.h stringbuilder.c)
