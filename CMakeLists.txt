cmake_minimum_required(VERSION 3.26)
project(fmenu)

set(CMAKE_CXX_STANDARD 20)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)

file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp)
add_executable(${PROJECT_NAME}
        ${IMGUI_SOURCES}
        menu/main.cpp
        menu/dx11.cpp
        menu/dx11.h
        menu/render.cpp
        fc2.hpp
)
target_link_libraries(${PROJECT_NAME} dwmapi d3d11 dxgi d3dcompiler)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")