#pragma once

#include <cstdint>
#include "../AppOptions.h"

struct GLFWwindow;

namespace AppGL
{
        class Window
        {
                GLFWwindow* window;
                uint32_t width, height;
        
        public:
                explicit Window(const AppOptions& options);
                explicit operator GLFWwindow*() const { return window; }
        };
}