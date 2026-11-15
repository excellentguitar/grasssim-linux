cmake_minimum_required(VERSION 3.20)

project(grasssim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RAYLIB_DIR "C:/dev/raylib")

add_executable(grasssim
    main.cpp

    src/World.cpp
    src/Tile.cpp
    src/Grass.cpp
    src/Renderer.cpp
)

target_include_directories(grasssim PRIVATE
    ${RAYLIB_DIR}/include
)

target_link_directories(grasssim PRIVATE
    ${RAYLIB_DIR}/lib
)

target_link_libraries(grasssim PRIVATE
    raylib
    winmm
    gdi32
    opengl32
)