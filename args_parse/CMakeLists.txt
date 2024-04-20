# в современном CMake рекомендуетс¤ сразу задавать нужную версию CMake.
cmake_minimum_required(VERSION 3.27)

# говорим CMake что за проект.
project(args_parse_lib LANGUAGES CXX)

# определяем библиотеку и указываем из чего она состоит.
add_library(args_parse STATIC ForwardDeclaration.hpp argument.hpp ArgsParser.cpp ArgsParser.hpp)
add_compile_options(/utf-8)

target_include_directories(args_parse PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_compile_features(args_parse PUBLIC cxx_std_17)