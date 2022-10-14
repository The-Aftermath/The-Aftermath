cmake_minimum_required(VERSION 3.13)

project(TheAftermath)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(NVRHI_WITH_VULKAN OFF CACHE BOOL "" FORCE)
set(NVRHI_WITH_SHADER_COMPILER OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/nvrhi)
add_subdirectory(source)