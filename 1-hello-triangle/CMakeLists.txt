cmake_minimum_required(VERSION 3.10.0)
project(grfx_lab VERSION 0.1.0 LANGUAGES C CXX)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

add_subdirectory(vendor/glfw)
include_directories(vendor/glfw/include vendor/glad/include)

add_executable(grfx_lab main.cpp vendor/glad/src/glad.c)
target_link_libraries(grfx_lab glfw)

