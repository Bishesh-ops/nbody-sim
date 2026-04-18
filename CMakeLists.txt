cmake_minimum_required(VERSION 3.20)

project(NBodySimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(nbody_sim src/main.cpp)

target_include_directories(nbody_sim PRIVATE include)

target_compile_options(nbody_sim PRIVATE -Wall -Wextra -Wpedantic)

find_package(SFML 2 COMPONENTS graphics window system REQUIRED)
target_link_libraries(nbody_sim PRIVATE sfml-graphics sfml-window sfml-system)