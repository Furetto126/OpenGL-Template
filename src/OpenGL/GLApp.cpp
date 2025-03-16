#include "GLApp.h"

#include <string>

#include "Window.h"
#include "glDebug.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLApp::GLApp(uint32_t width, uint32_t height, const std::vector<int>& extensions): window(Window(width, height))
{
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
                throw std::runtime_error("Failed to initialize GLAD");
        
        // Enable Debug
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback(glDebugOutput, nullptr);
        }
        
        // Check extensions
        for (auto extension : extensions)
                if (!extension)
                        throw std::runtime_error("Support for extension \"" + std::to_string(extension) + "\"" + " not found.");
}
