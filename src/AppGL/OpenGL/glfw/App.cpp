#include "App.h"

#include <stdexcept>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>

#include "../glDebug.h"

namespace AppGL
{
        App::App(AppOptions options): window(Window(options))
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
                if (!options.extensions.has_value()) return;
                for (auto extension : options.extensions.value())
                        if (!extension)
                                throw std::runtime_error(
                                        "Support for extension \"" +
                                        std::to_string(extension) + "\"" + " not found."
                                );
        }
        
        void App::run()
        {
                onStart();
                while (!glfwWindowShouldClose((GLFWwindow*)window)) {
                        glfwPollEvents();
                        onUpdate();
                        glfwSwapBuffers((GLFWwindow*)window);
                }
                
                onDestroy();
                
                glfwDestroyWindow((GLFWwindow*)window);
                glfwTerminate();
        }
        
        void App::close() const { glfwSetWindowShouldClose((GLFWwindow*)window, true); }
}

int main()
{
        AppGL::App* app = AppGL::InitApp();
        app->run();
        delete app;
        return 0;
}