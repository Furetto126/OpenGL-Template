#pragma once

#include <cstdint>
#include <glm/vec2.hpp>

#include "../AppOptions.h"

struct GLFWwindow;

namespace AppGL
{
        class Window
        {
                GLFWwindow* window;
                uint32_t width, height;

                void updateWindowSize() const;
                static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        public:
                explicit Window(const AppOptions& options);
                explicit operator GLFWwindow*() const { return window; }
                static float getTime();

                void setResolution(const glm::uvec2 newResolution);
                void setWidth(const uint32_t newWidth);
                void setHeight(const uint32_t newHeight);

                [[nodiscard]] glm::vec2 getResolution() const;
                [[nodiscard]] uint32_t  getWidth()      const { return width; }
                [[nodiscard]] uint32_t  getHeight()     const { return height; }

        };
}