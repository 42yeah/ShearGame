cmake_minimum_required(VERSION 2.8.11)

project(Shear3D)

set(CMAKE_CXX_FLAGS --std=c++11)

# Gotta find GLFW first
# find_package(glfw3 3.2 REQUIRED)

add_subdirectory(Includes/glfw)

add_subdirectory(Includes/glm)

# Then make imgui
add_subdirectory(Includes/imgui)

# Then our project
add_subdirectory(Shear3D)
