#pragma once

#include <cstdint>

struct GLFWwindow;

class Window
{
        GLFWwindow* window;
        uint32_t width, height;
        
public:
        Window(uint32_t width, uint32_t height);
        
        explicit operator GLFWwindow*() const { return window; }
};