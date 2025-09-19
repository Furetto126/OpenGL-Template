#pragma once

#include <vector>
#include <optional>

namespace AppGL
{
        struct AppOptions
        {
                uint32_t width, height;
                const char* title;
                std::optional<std::pair<int, int>> position;
                
                // OpenGL extensions.
                std::optional<std::vector<int>> extensions;
                // Pixel format attributes WGL.
                std::optional<std::vector<std::pair<int, int>>> pixelFormatAttributes;
                // OpenGL attributes.
                std::optional<std::vector<std::pair<int, int>>> GLAttributes;
        };
}