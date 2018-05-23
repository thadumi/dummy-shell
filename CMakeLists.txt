cmake_minimum_required(VERSION 3.9)
project(shell C)

set(CMAKE_C_STANDARD 11)

add_executable(shell main.c headers/loop_core.h headers/process.h sources/loop_core.c headers/shell.h sources/shell.c headers/util/utiliteas.h sources/util/collections/linkedl.c headers/util/collections/linkedl.h sources/process.c sources/logger.c headers/logger.h sources/util/utiliteas.c)