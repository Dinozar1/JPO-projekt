cmake_minimum_required(VERSION 3.8)
project(weather_app)

add_executable(weather_app main.cpp
        UseImGui.cpp
        UseImGui.h)

# Include the correct path to ImGui headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-linux/include)

# Find required packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Link libraries
target_link_libraries(weather_app PRIVATE
        glfw
        glad::glad
        imgui::imgui
        ${CMAKE_DL_LIBS}  # For dlopen, etc.
)