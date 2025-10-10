#include "Window.h"

#include <stdexcept>

#include <glad/glad.h>
#include "GLFW/glfw3.h"

namespace AppGL
{
        Window::Window(const AppOptions& options): width(options.width), height(options.height)
        {
                glfwInit();
                
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
                
                if (options.pixelFormatAttributes.has_value()) {
                        for (const auto& [flag, value] : options.pixelFormatAttributes.value())
                                glfwWindowHint(flag, value);
                }
                
                window = glfwCreateWindow(
                        (int)width, (int)height, options.title,
                        nullptr, nullptr
                );
                
                if (!window) {
                        glfwTerminate();
                        throw std::runtime_error("Failed to create GLFW window.");
                }
                
                if (options.position.has_value())
                        glfwSetWindowPos(window, options.position.value().first, options.position.value().second);
                
                glfwMakeContextCurrent(window);
                glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
                glfwSwapInterval(0);
        }

        void Window::framebufferSizeCallback(GLFWwindow* window, int newWidth, int newHeight)
        {
                glViewport(0, 0, newWidth, newHeight);
        }

        float Window::getTime() const
        {
                return (float)glfwGetTime();
        }

        glm::vec2 Window::getResolution() const
        {
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                return { width, height };
        }
}
