# В современном CMake рекомендуется сразу задавать нужную версию CMake.
cmake_minimum_required(VERSION 3.28)

# Говорим CMake что за проект.
project(args_parse_demo_app LANGUAGES CXX)

# Определяем исполнимый файл и из чего он состоит.
#add_library(directory_travers STATIC DirectoryTravers.hpp)

add_executable(directory_travers_demo DirectoryTravers.hpp main.cpp)

add_compile_options(/utf-8)

# Библиотека args_parse должна быть прилинкована к этому исполнимому файлу.
target_link_libraries(directory_travers_demo PRIVATE directory_travers args_parse)

target_compile_features(directory_travers_demo PUBLIC cxx_std_17)