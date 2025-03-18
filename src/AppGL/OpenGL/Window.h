#pragma once

#include <cstdint>

struct GLFWwindow;
namespace AppGL
{
    class Window
    {
            GLFWwindow* window;
            uint32_t width, height;
    
    public:
            Window(uint32_t width, uint32_t height, const char* title);
            explicit operator GLFWwindow*() const { return window; }
    };
}