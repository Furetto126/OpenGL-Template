#pragma once

#include <vector>
#include <optional>

#include <gsprop.h>

#include "Window.h"

namespace AppGL
{
        struct AppOptions
        {
                uint32_t width, height;
                const char* title;
                std::optional<std::vector<int>> extensions;
        };
        
        class App
        {
        public:
                Window window;
                
                explicit App(AppOptions options);
                virtual ~App() = default;
            
                virtual void onStart()   = 0;
                virtual void onUpdate()  = 0;
                virtual void onDestroy() = 0;
            
                void run();
                void close() const;
        };
    
        extern App* InitApp();
}