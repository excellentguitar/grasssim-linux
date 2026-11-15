cmake_minimum_required(VERSION 3.16)

project(grassim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED IMPORTED_TARGET raylib)

add_executable(grassim
    main.cpp
    src/World.cpp
    src/Tile.cpp
    src/Grass.cpp
    src/Renderer.cpp
)

target_link_libraries(grassim PRIVATE PkgConfig::RAYLIB)
