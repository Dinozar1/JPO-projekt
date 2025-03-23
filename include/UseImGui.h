#pragma once

#include <../vcpkg_installed/x64-linux/include/imgui.h>
#include <../cmake-build-release/vcpkg_installed/x64-linux/include/imgui_impl_glfw.h>
#include <../cmake-build-release/vcpkg_installed/x64-linux/include/imgui_impl_opengl3.h>

class UseImGui {
public:
        void Init(GLFWwindow* window, const char* glsl_version);
        void NewFrame();
        virtual void Update();
        void Render();
        void Shutdown();
};