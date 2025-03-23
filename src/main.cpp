/**
* @file main.cpp
 * @brief Main entry point for the Air Quality Monitoring application
 * @author Szymon
 */

#include "AirQualityMonitor.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    // Setup OpenGL version and profile
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(
        1280, 720,
        "Air Quality Monitoring - GIOŚ API",
        NULL, NULL
    );

    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return 1;
    }

    // Get framebuffer size and set viewport
    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);

    // Create and initialize the app
    AirQualityMonitor app;
    app.Init(window, glsl_version);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Process events
        glfwPollEvents();

        // Clear the framebuffer
        glClear(GL_COLOR_BUFFER_BIT);

        // Start new ImGui frame and update the UI
        app.NewFrame();
        app.Update();
        app.Render();

        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

    // Clean up
    app.Shutdown();
    glfwTerminate();

    return 0;
}