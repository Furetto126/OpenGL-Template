#include "Window.h"

#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(uint32_t width, uint32_t height): width(width), height(height)
{
        glfwInit();
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        
        window = glfwCreateWindow(
                width, height,
                "OpenGL Application",
                nullptr, nullptr
        );
        
        if (!window) {
                glfwTerminate();
                throw std::runtime_error("Failed to create GLFW window.");
        }
        
        glfwMakeContextCurrent(window);
}
