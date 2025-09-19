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

                static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        public:
                explicit Window(const AppOptions& options);
                explicit operator GLFWwindow*() const { return window; }
                float getTime() const;
        };
}