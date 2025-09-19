#include "Win32App.h"

#include <glad/glad.h>
#include <utility>

#include "../glDebug.h"

namespace AppGL
{
        Win32App::Win32App(HINSTANCE hInstance, AppOptions options)
                : window(Win32Window(hInstance, std::move(options)))
        {
                if (!gladLoadGL())
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
        
        void Win32App::run()
        {
                onStart();
                
                while (!window.shouldClose) {
                        window.pollEvents();
                        
                        onUpdate();
                        
                        window.swapBuffers();
                }
                
                onDestroy();
        }
        
        void Win32App::close() { window.shouldClose = true; }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
        AppGL::Win32App* nativeApp = AppGL::InitApp(hInstance);
        nativeApp->run();
        delete nativeApp;
        
        return 0;
}