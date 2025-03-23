cmake_minimum_required(VERSION 3.8)
project(weather_app)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files
set(SOURCES
        src/main.cpp
        src/UseImGui.cpp
        src/AirQualityMonitor.cpp
        src/Station.cpp
        src/Utils.cpp
)

# Define header files
set(HEADERS
        include/UseImGui.h
        include/Station.h
        include/AirQualityMonitor.h
        include/Utils.h
)

# Create the executable
add_executable(weather_app ${SOURCES} ${HEADERS})

# Include directories
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-linux/include
)

# Find required packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)  # Find implot package
find_package(OpenGL REQUIRED)         # Find OpenGL package

# Link libraries
target_link_libraries(weather_app PRIVATE
        glfw
        glad::glad
        imgui::imgui
        CURL::libcurl
        nlohmann_json::nlohmann_json
        implot::implot                # Link implot
        OpenGL::GL                    # Link OpenGL
        ${CMAKE_DL_LIBS}  # For dlopen, etc.
)