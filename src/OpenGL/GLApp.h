#pragma once

#include <vector>

#include "Window.h"

class GLApp
{
public:
        Window window;
        GLApp(uint32_t width, uint32_t height, const std::vector<int>& extensions = {});
};