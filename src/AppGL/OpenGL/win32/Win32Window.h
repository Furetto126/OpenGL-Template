#pragma once

#include <Windows.h>
#include "../AppOptions.h"

namespace AppGL
{
        class Win32Window
        {
        public:
                Win32Window(HINSTANCE hInstance, AppOptions options);
                ~Win32Window();
                
                void pollEvents();
                void swapBuffers() const { SwapBuffers(deviceContext); }
                bool shouldClose = false;
                
                Win32Window(const Win32Window&) = delete;
                Win32Window& operator=(const Win32Window&) = delete;
        private:
                HWND  windowHandle;
                HDC   deviceContext;
                HGLRC GLContext;
                
                AppOptions options;
                
                static LRESULT CALLBACK windowCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
                
                [[nodiscard]] HWND createWindow(HINSTANCE instance) const;
                static void initGLExtensions();
                HGLRC initGL();
        };
}