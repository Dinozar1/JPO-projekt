cmake_minimum_required(VERSION 3.8)
project(weather_app)

add_executable(weather_app main.cpp
        UseImGui.cpp
        UseImGui.h
        apiCalls/getAllStations.cpp
        apiCalls/getAllStations.h
)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-linux/include)

# Find required packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)
find_package(leaflet-cpp CONFIG REQUIRED)

# Link libraries
target_link_libraries(weather_app PRIVATE
        glfw
        glad::glad
        imgui::imgui
        CURL::libcurl
        nlohmann_json::nlohmann_json
        stb::stb
        leaflet-cpp::leaflet-cpp
        ${CMAKE_DL_LIBS}
)